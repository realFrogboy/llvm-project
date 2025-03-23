//===-- RArchMCTargetDesc.cpp - RArch Target Descriptions -------------------===//
//
//                     The LLVM Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This file provides RArch specific target descriptions.
//
//===----------------------------------------------------------------------===//

#include "RArchInfo.h"
#include "RArchMCTargetDesc.h"
#include "TargetInfo/RArchTargetInfo.h"
#include "RArchInstPrinter.h"
#include "RArchElfStreamer.h"
#include "RArchObjectFileInfo.h"
#include "RArchMCAsmInfo.h"
#include "RArchTargetStreamer.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCInstrAnalysis.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCObjectFileInfo.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

#define GET_REGINFO_ENUM
#define GET_REGINFO_MC_DESC
#include "RArchGenRegisterInfo.inc"

#define ENABLE_INSTR_PREDICATE_VERIFIER
#define GET_INSTRINFO_ENUM
#define GET_INSTRINFO_MC_DESC
#include "RArchGenInstrsInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "RArchGenSubtargetInfo.inc"

static MCInstrInfo *createRArchMCInstrInfo() {
  auto *X = new MCInstrInfo();
  InitRArchMCInstrInfo(X);
  return X;
}

static MCRegisterInfo *createRArchMCRegisterInfo(const Triple &TT) {
  auto *X = new MCRegisterInfo();
  InitRArchMCRegisterInfo(X, rarch::X1);
  return X;
}

static MCSubtargetInfo *createRArchMCSubtargetInfo(const Triple &TT,
                                                    StringRef CPU, StringRef FS) {
  return createRArchMCSubtargetInfoImpl(TT, CPU, /*TuneCPU=*/CPU, FS);
}

static MCAsmInfo *createRArchMCAsmInfo(const MCRegisterInfo &MRI,
                                        const Triple &TT,
                                        const MCTargetOptions &Options) {
  MCAsmInfo *MAI = new RArchMCAsmInfo(TT);
  MCRegister SP = MRI.getDwarfRegNum(rarch::X2, true);
  MCCFIInstruction Inst = MCCFIInstruction::cfiDefCfa(nullptr, SP, 0);
  MAI->addInitialFrameState(Inst);
  return MAI;
}

static MCInstPrinter *createRArchMCInstPrinter(const Triple &T,
                                                unsigned SyntaxVariant,
                                                const MCAsmInfo &MAI,
                                                const MCInstrInfo &MII,
                                                const MCRegisterInfo &MRI) {
  return new RArchInstPrinter(MAI, MII, MRI);
}

static MCTargetStreamer *createRArchTargetAsmStreamer(MCStreamer &S,
                                                       formatted_raw_ostream &OS,
                                                       MCInstPrinter *InstPrint) {
  return new RArchTargetStreamer(S);
}

static MCObjectFileInfo *
createRArchMCObjectFileInfo(MCContext &Ctx, bool PIC,
                             bool LargeCodeModel = false) {
  MCObjectFileInfo *MOFI = new RArchMCObjectFileInfo();
  MOFI->initMCObjectFileInfo(Ctx, PIC, LargeCodeModel);
  return MOFI;
}

static MCTargetStreamer *
createRArchObjectTargetStreamer(MCStreamer &S, const MCSubtargetInfo &STI) {
  const Triple &TT = STI.getTargetTriple();
  if (TT.isOSBinFormatELF())
    return new RArchTargetELFStreamer(S, STI);
  return nullptr;
}

class RArchMCInstrAnalysis : public MCInstrAnalysis {
public:
  explicit RArchMCInstrAnalysis(const MCInstrInfo *Info)
      : MCInstrAnalysis(Info) {}

  bool evaluateBranch(const MCInst &Inst, uint64_t Addr, uint64_t Size,
                      uint64_t &Target) const override {
    if (isConditionalBranch(Inst)) {
      int64_t Imm;
      if (Size == 2)
        Imm = Inst.getOperand(1).getImm();
      else
        Imm = Inst.getOperand(2).getImm();
      Target = Addr + Imm;
      return true;
    }

    if (Inst.getOpcode() == rarch::JAL) {
      Target = Addr + Inst.getOperand(1).getImm();
      return true;
    }

    return false;
  }
};

static MCInstrAnalysis *createRArchInstrAnalysis(const MCInstrInfo *Info) {
  return new RArchMCInstrAnalysis(Info);
}

static MCTargetStreamer *createRArchNullTargetStreamer(MCStreamer &S) {
  return new RArchTargetStreamer(S);
}

namespace {
MCStreamer *createRArchELFStreamer(const Triple &T, MCContext &Context,
                                    std::unique_ptr<MCAsmBackend> &&MAB,
                                    std::unique_ptr<MCObjectWriter> &&MOW,
                                    std::unique_ptr<MCCodeEmitter> &&MCE) {
  return createRArchELFStreamer(Context, std::move(MAB), std::move(MOW),
                                 std::move(MCE));
}
} // end anonymous namespace

// Force static initialization.
extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeRArchTargetMC() {
  // Register the MC asm info.
  Target &TheRArchTarget = getTheRArchTarget();
  RegisterMCAsmInfoFn X(TheRArchTarget, createRArchMCAsmInfo);

  // Register the MC instruction info.
  TargetRegistry::RegisterMCObjectFileInfo(TheRArchTarget, createRArchMCObjectFileInfo);
  TargetRegistry::RegisterMCInstrInfo(TheRArchTarget, createRArchMCInstrInfo);
  // Register the MC register info.
  TargetRegistry::RegisterMCRegInfo(TheRArchTarget, createRArchMCRegisterInfo);

  TargetRegistry::RegisterMCAsmBackend(TheRArchTarget, createRArchAsmBackend);
  TargetRegistry::RegisterMCCodeEmitter(TheRArchTarget, createRArchMCCodeEmitter);
  TargetRegistry::RegisterMCInstPrinter(TheRArchTarget, createRArchMCInstPrinter);
  // Register the MC subtarget info.
  TargetRegistry::RegisterMCSubtargetInfo(TheRArchTarget,
                                          createRArchMCSubtargetInfo);
  TargetRegistry::RegisterELFStreamer(TheRArchTarget, createRArchELFStreamer);
  TargetRegistry::RegisterObjectTargetStreamer(TheRArchTarget,
                                               createRArchObjectTargetStreamer);
  TargetRegistry::RegisterMCInstrAnalysis(TheRArchTarget, createRArchInstrAnalysis);
  // Register the MCInstPrinter
  TargetRegistry::RegisterMCInstPrinter(TheRArchTarget, createRArchMCInstPrinter);

  TargetRegistry::RegisterAsmTargetStreamer(TheRArchTarget,
                                            createRArchTargetAsmStreamer);

  TargetRegistry::RegisterNullTargetStreamer(TheRArchTarget,
                                               createRArchNullTargetStreamer);
}
