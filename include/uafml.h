#ifndef AUF_H
#define AUF_H

#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Value.h"

namespace uafml {

struct UAFMLAnalysis : public llvm::AnalysisInfoMixin<UAFMLAnalysis> {
	//using Result = llvm::SmallVector<llvm::Instruction *, 0>;
	using Result = std::vector<int>;
	//std::vector<int> Result = {};
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
