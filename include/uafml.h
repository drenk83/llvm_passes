#ifndef UAFML_H
#define UAFML_H

#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Value.h"
#include <vector>
#include <set>

namespace uafml {

struct UAFMLAnalysis : public llvm::AnalysisInfoMixin<UAFMLAnalysis> {
    struct Result {
        std::vector<int> flags; // 0 - MLFlag, 1 - UAFFlag
        std::set<unsigned> freedArgs; // Индексы аргументов, которые функция освобождает
    };
    Result run(llvm::Function &F, llvm::FunctionAnalysisManager &FAM);
    static llvm::AnalysisKey Key;
};

struct UAFMLPrinterPass : public llvm::PassInfoMixin<UAFMLPrinterPass> {
    explicit UAFMLPrinterPass(llvm::raw_ostream &OS) : OS(OS) {}
    llvm::PreservedAnalyses run(llvm::Function &F, llvm::FunctionAnalysisManager &FAM);

private:
    llvm::raw_ostream &OS;
};

}

#endif