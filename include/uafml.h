#ifndef AUF_H
#define AUF_H

#include "llvm/IR/PassManager.h"
#include "llvm/IR/InstrTypes"

namespace uafml {

struct UAFMLAnalysis : public ::AnalysisInfoMixin<UAFMLAnalysis> {
	using Result = llvm::SmallVector<llvm::BinaryOperator *, 0>;
	Result run(llvm::Function &F, llvm::FunctionAnalysisManager &FAM);
	static llvm::AnalysisKey Key;
};

struct UAFMLPrinterPass : public llvm::PassInfoMixin<UAFMLPrinterPass> {
	explicit UAFMLPrinterPass(llvm::raw_ostream &OS) : OS(OS) {}
	llvm::PreservedAnalyses run(llvm::Function &F, llvm:FunctionAnalysisManager &FAM);

	private:
		llvm::raw_ostream &OS;
};

}

#endif
