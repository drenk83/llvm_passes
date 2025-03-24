#include "uafml.h"
#include <algorithm>
#include "llvm/IR/Constants.h"
#include "llvm/Support/Casting.h"
#include "llvm/IR/Attributes.h"
#include <map>
#include <set>

using namespace llvm;

namespace uafml {

AnalysisKey UAFMLAnalysis::Key;

UAFMLAnalysis::Result UAFMLAnalysis::run(llvm::Function &F, llvm::FunctionAnalysisManager &FAM) {
    Result res;
    res.flags = {0, 0}; // 0 - MLFlag, 1 - UAFFlag

    std::map<Value*, std::set<Value*>> mallocPointers; // Указатели, связанные с каждым malloc
    std::map<unsigned, std::set<Value*>> argPointers;  // Указатели, связанные с каждым аргументом
    std::set<Value*> freedPointers;                    // Освобожденные указатели
    std::map<Instruction*, std::set<Value*>> freedAfter; // Указатели, освобожденные после инструкции

    for (unsigned i = 0; i < F.arg_size(); ++i) {
        Value *arg = F.getArg(i);
        argPointers[i].insert(arg);
    }

    bool changed = true;
    while (changed) {
        changed = false;
        for (BasicBlock &BB : F) {
            for (Instruction &I : BB) {
                if (auto *Call = dyn_cast<CallInst>(&I)) {
                    Function *Callee = Call->getCalledFunction();
                    if (Callee && Callee->getName() == "malloc") {
                        Value *mallocResult = Call;
                        if (mallocPointers[mallocResult].insert(mallocResult).second) {
                            changed = true;
                        }
                    }
                }
                if (auto *Store = dyn_cast<StoreInst>(&I)) {
                    Value *valueOp = Store->getValueOperand();
                    Value *pointerOp = Store->getPointerOperand();
                    for (auto &mp : mallocPointers) {
                        if (mp.second.count(valueOp) && mp.second.insert(pointerOp).second) {
                            changed = true;
                        }
                    }
                    for (auto &ap : argPointers) {
                        if (ap.second.count(valueOp) && ap.second.insert(pointerOp).second) {
                            changed = true;
                        }
                    }
                }
                if (auto *Load = dyn_cast<LoadInst>(&I)) {
                    Value *pointerOp = Load->getPointerOperand();
                    for (auto &mp : mallocPointers) {
                        if (mp.second.count(pointerOp) && mp.second.insert(Load).second) {
                            changed = true;
                        }
                    }
                    for (auto &ap : argPointers) {
                        if (ap.second.count(pointerOp) && ap.second.insert(Load).second) {
                            changed = true;
                        }
                    }
                }
            }
        }
    }

    for (BasicBlock &BB : F) {
        for (Instruction &I : BB) {
            if (auto *Call = dyn_cast<CallInst>(&I)) {
                Function *Callee = Call->getCalledFunction();
                if (Callee) {
                    if (Callee->getName() == "free") {
                        Value *arg = Call->getArgOperand(0);
                        for (auto &mp : mallocPointers) {
                            if (mp.second.count(arg)) {
                                freedPointers.insert(mp.first);
                                freedAfter[&I].insert(mp.first);
                            }
                        }
                        for (auto &ap : argPointers) {
                            if (ap.second.count(arg)) {
                                res.freedArgs.insert(ap.first);
                            }
                        }
                    } else if (Callee->getName() != "malloc") {
                        auto &calleeRes = FAM.getResult<UAFMLAnalysis>(*Callee);
                        for (unsigned i : calleeRes.freedArgs) {
                            if (i < Call->arg_size()) {
                                Value *arg = Call->getArgOperand(i);
                                for (auto &mp : mallocPointers) {
                                    if (mp.second.count(arg)) {
                                        freedPointers.insert(mp.first);
                                        freedAfter[&I].insert(mp.first);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    for (BasicBlock &BB : F) {
        for (Instruction &I : BB) {
            std::set<Value*> freedBefore;
            for (auto &fa : freedAfter) {
                if (fa.first->comesBefore(&I)) {
                    freedBefore.insert(fa.second.begin(), fa.second.end());
                }
            }
            for (unsigned i = 0; i < I.getNumOperands(); ++i) {
                Value *op = I.getOperand(i);
                for (Value *freed : freedBefore) {
                    if (mallocPointers[freed].count(op)) {
                        res.flags[1] = 1; // UAFFlag
                    }
                }
            }
        }
    }

    // Проверка MemoryLeak
    int mallocCount = mallocPointers.size();
    int freeCount = freedPointers.size();
    if (mallocCount > freeCount) {
        res.flags[0] = 1; // MLFlag
    }

    return res;
}

PreservedAnalyses UAFMLPrinterPass::run(Function &F, FunctionAnalysisManager &FAM) {
    auto &res = FAM.getResult<UAFMLAnalysis>(F);

    if (res.flags[0] == 1 || res.flags[1] == 1) {
        OS << "Function name: " << F.getName() << "\n";
        if (res.flags[0] == 1) OS << "\tWARNING: Memory Leak detection\n";
        if (res.flags[1] == 1) OS << "\tWARNING: Use After Free detection\n";
    } else {
        OS << "Function " << F.getName() << ": Everything is fine!\n";
    }

    return PreservedAnalyses::all();
}

}