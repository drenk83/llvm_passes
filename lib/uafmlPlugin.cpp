#include "uafml.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

using namespace llvm;

void registerAnalyses(FunctionAnalysisManager &FAM) {
	FAM.registerPass([] { return uafml::UAFMLAnalysis(); });
}

bool registerPipeline(StringRef Name, FunctionPassManager &FPM,
                      ArrayRef<PassBuilder::PipelineElement>) {
    if (Name == "<uafml>") {
        FPM.addPass(uafml::UAFMLPrinterPass(errs()));
        return true;
    }
    return false;
}

PassPluginLibraryInfo getUAFMLPluginInfo() {
    return {LLVM_PLUGIN_API_VERSION, "uafml", LLVM_VERSION_STRING,
            [](PassBuilder &PB) {
                PB.registerAnalysisRegistrationCallback(registerAnalyses);
                PB.registerPipelineParsingCallback(registerPipeline);
            }};
}

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
    return getUAFMLPluginInfo();
}