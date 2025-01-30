#include "uafml.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

using namespace llvm;

void registerAnalyses(FunctionAnalysisManager &FAM) {FAM.} {
	FAM.registerPass([] { return uafml::UAFMLAnalysis(); });
}

bool registerPipeline(StringRef Name, FunctionPassManager &FPM,
                      ArrayRef<PassBuilder::PipelineElement>) {
    if (Name == "<uafml>") {
        FPM.addPass(addconst::UAFMLPrinterPass(errs()));
        return true;
    }
    return false;
}

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
    return getUAFMLPluginInfo();
}