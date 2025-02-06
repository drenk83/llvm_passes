#include "uafml.h"
#include <algorithm>
#include "llvm/IR/Constants.h"
#include "llvm/Support/Casting.h"
#include "llvm/IR/Attributes.h"
#include <map>

using namespace llvm;

namespace uafml {

    AnalysisKey UAFMLAnalysis::Key;

    UAFMLAnalysis::Result UAFMLAnalysis::run(llvm::Function &F, llvm::FunctionAnalysisManager &FAM){
        int MLFlag = 0, UAFFlag = 0, mallocFlag = 0, mallocCount = 0, freeCount = 0;
        llvm::Instruction *PrevInst;
        std::vector<int> OutRes;
        std::map<int, std::vector<Value*>> mallocOperands;
        std::vector<Value*> freeOperands;
        Value *PointerOperand;
        Value *loadOperand, *storeOperand;
        for (BasicBlock &BB : F) {
            for (Instruction &I : BB) {
                for (unsigned i = 0; i < I.getNumOperands(); i++) {
                    Value *Operand = I.getOperand(i);
                    if (std::find(freeOperands.begin(), freeOperands.end(), Operand) != freeOperands.end()) {
                        UAFFlag = 1;
                    }
                }

                if (auto *Store = dyn_cast<StoreInst>(&I)) {
                    if (auto *Load = dyn_cast<LoadInst>(PrevInst)) {
                        storeOperand = Store->getPointerOperand();
                        loadOperand = Load->getPointerOperand();

                        for (auto &loadop : mallocOperands) {
                            if (std::find(loadop.second.begin(), loadop.second.end(), loadOperand) != loadop.second.end()) {
                                mallocOperands[loadop.first].push_back(storeOperand);
                            }
                        } 
                    }
                }

                if(mallocFlag){
                    auto *Store = dyn_cast<StoreInst>(&I);
                    PointerOperand = Store->getPointerOperand();
                    mallocOperands[mallocCount].push_back(PointerOperand);
                    mallocCount++;
                    mallocFlag--;
                }

                if (auto *Call = dyn_cast<CallInst>(&I)) {
                    llvm::Function *Callee = Call->getCalledFunction();
                    if (Callee && Callee->getName() == "malloc") {
                        mallocFlag++;
                    }

                    if (Callee && Callee->getName() == "free") {
                        auto *Load = dyn_cast<LoadInst>(PrevInst);
                        PointerOperand = Load->getPointerOperand();
                        for (auto &indx : mallocOperands) {
                            if (std::find(indx.second.begin(), indx.second.end(), PointerOperand) != indx.second.end()) {
                                if (std::find(freeOperands.begin(), freeOperands.end(), PointerOperand) == freeOperands.end())
                                    freeCount++;
                                for (auto &oper : indx.second) {
                                    freeOperands.push_back(oper);
                                }
                            }
                        }
                    }
                }
                PrevInst = &I;
            }
            if(mallocCount != freeCount) {
                MLFlag = 1;
            }
        }
        OutRes.push_back(MLFlag);
        OutRes.push_back(UAFFlag);
        return OutRes;
    }

    PreservedAnalyses UAFMLPrinterPass::run(Function &F,
                                           FunctionAnalysisManager &FAM) {
        auto &OutRes = FAM.getResult<UAFMLAnalysis>(F);
        if (OutRes[0] == 1 || OutRes[1] == 1) OS << "Function name: " << F.getName() << "\n";
        if (OutRes[0] == 1) OS << "\tWARNING: Memory Leak detection\n";
        if (OutRes[1] == 1) OS << "\tWARNING: Use After Free detection\n";
        return PreservedAnalyses::all();
    }
}