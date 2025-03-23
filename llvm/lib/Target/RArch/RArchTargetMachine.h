#ifndef __LLVM_LIB_TARGET_SIM_SIMTARGETMACHINE_H__
#define __LLVM_LIB_TARGET_SIM_SIMTARGETMACHINE_H__

#include "RArchSubtarget.h"
#include "MCTargetDesc/RArchMCTargetDesc.h"
#include "llvm/CodeGen/SelectionDAGTargetInfo.h"
#include "llvm/CodeGen/CodeGenTargetMachineImpl.h"
#include "llvm/IR/DataLayout.h"
#include <optional>

namespace llvm {

class RArchTargetMachine : public CodeGenTargetMachineImpl {
  std::unique_ptr<TargetLoweringObjectFile> TLOF;
  RArchSubtarget Subtarget;

public:
  RArchTargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                     StringRef FS, const TargetOptions &Options,
                     std::optional<Reloc::Model> RM, std::optional<CodeModel::Model> CM,
                     CodeGenOptLevel OL, bool JIT);
  ~RArchTargetMachine() override;

  const RArchSubtarget *getSubtargetImpl() const { return &Subtarget; }
  const RArchSubtarget *getSubtargetImpl(const Function &) const override {
    return &Subtarget;
  }

  // Pass Pipeline Configuration
  TargetPassConfig *createPassConfig(PassManagerBase &PM) override;
  TargetLoweringObjectFile *getObjFileLowering() const override {
    return TLOF.get();
  }

  MachineFunctionInfo *
  createMachineFunctionInfo(BumpPtrAllocator &Allocator, const Function &F,
                            const TargetSubtargetInfo *STI) const override;
};

FunctionPass *createRArchISelDag(RArchTargetMachine &TM,
                                  CodeGenOptLevel OptLevel);

} // end namespace llvm

#endif // __LLVM_LIB_TARGET_SIM_SIMTARGETMACHINE_H__
