#include "uafml.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

using namespace llvm;

void registerAnalyses(FunctionAnalysisManager &FAM) {FAM.} {
	FAM.registerPass([] { return uafml::UAFMLAnalysis(); });
}

bool  registerPipeline(StringRef Name)
