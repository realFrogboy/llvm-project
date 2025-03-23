//===----------------------------------------------------------------------===//
//
// Implements the info about RArch target spec.
//
//===----------------------------------------------------------------------===//

#include "RArchTargetMachine.h"
#include "RArchMachineFunctionInfo.h"
#include "TargetInfo/RArchTargetInfo.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/CodeGen.h"

#define DEBUG_TYPE "sim"

using namespace llvm;

static Reloc::Model getEffectiveRelocModel(const Triple &TT,
                                           std::optional<Reloc::Model> RM) {
  return RM.value_or(Reloc::Static);
}

/// simTargetMachine ctor - Create an LP64 Architecture model
RArchTargetMachine::RArchTargetMachine(const Target &T, const Triple &TT,
                                         StringRef CPU, StringRef FS,
                                         const TargetOptions &Options,
                                         std::optional<Reloc::Model> RM,
                                         std::optional<CodeModel::Model> CM,
                                         CodeGenOptLevel OL, bool JIT)
    : CodeGenTargetMachineImpl(T, "e-m:e-p:64:64-i64:64-i128:128-n32:64-S128",
                        TT, CPU, FS, Options, getEffectiveRelocModel(TT, RM),
                        getEffectiveCodeModel(CM, CodeModel::Small), OL),
      TLOF(std::make_unique<TargetLoweringObjectFileELF>()),
      Subtarget(TT, std::string(CPU), std::string(FS), *this) {
  initAsmInfo();
}

RArchTargetMachine::~RArchTargetMachine() = default;

MachineFunctionInfo *RArchTargetMachine::createMachineFunctionInfo(
    BumpPtrAllocator &Allocator, const Function &F,
    const TargetSubtargetInfo *STI) const {
  return RArchFunctionInfo::create<RArchFunctionInfo>(Allocator, F, STI);
}

namespace {

class RArchPassConfig : public TargetPassConfig {
public:
  RArchPassConfig(RArchTargetMachine &TM, PassManagerBase &PM)
      : TargetPassConfig(TM, PM) {}

  RArchTargetMachine &getRArchTargetMachine() const {
    return getTM<RArchTargetMachine>();
  }

  bool addInstSelector() override;
};

} // anonymous namespace

TargetPassConfig *RArchTargetMachine::createPassConfig(PassManagerBase &PM) {
  return new RArchPassConfig(*this, PM);
}

bool RArchPassConfig::addInstSelector() {
  addPass(createRArchISelDag(getRArchTargetMachine(), getOptLevel()));
  return false;
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeRArchTarget() {
  RegisterTargetMachine<RArchTargetMachine> X(getTheRArchTarget());
}
