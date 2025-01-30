#include "uafml.h"
#include "llvm/IR/Constants.h"
#include "llvm/Support/Casting.h"

using namespace llvm;

namespace uafml {

    AnalysisKey UAFMLAnalysis::Key;

    bool isConstantIntOnly(Instruction &I) {
        for (Use &Op : I.operands()) {
            if (!isa<ConstantInt>(Op)) return false;
        }
        return true;
    }

    UAFMLAnalysis::Result UAFMLAnalysis::run(llvm::Function &F, llvm::FunctionAnalysisManager &FAM){
        SmallVector<BinaryOperator *, 0> AddInsts;
        for (BasicBlock &BB : F) {
            for (Instruction &I : BB) {
                if (!I.isBinaryOp()) continue;
                if (!(I.getOpcode() == Instruction::BinaryOps::Add)) continue;
                if (!isConstantIntOnly(I)) continue;

                AddInsts.push_back(&cast<BinaryOperator>(I));
            }
    }
    return AddInsts;
    }

    PreservedAnalyses UAFMLPrinterPass::run(Function &F,
                                           FunctionAnalysisManager &FAM) {
        auto &AddInsts = FAM.getResult<UAFMLAnalysis>(F);
        OS << "=============================================\n";
        OS << "|| " << F.getName() << " ||\n";
        OS << "=============================================\n";
        for (auto &Add : AddInsts) OS << *Add << "\n";
        OS << "=============================================\n";
        return PreservedAnalyses::all();
    }
}