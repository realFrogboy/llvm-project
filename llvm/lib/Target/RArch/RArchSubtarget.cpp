#include "RArchSubtarget.h"

using namespace llvm;

#define DEBUG_TYPE "rarch-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "RArchGenSubtargetInfo.inc"

void RArchSubtarget::anchor() {}

RArchSubtarget::RArchSubtarget(const Triple &TT, const std::string &CPU,
                             const std::string &FS, const TargetMachine &TM)
    : RArchGenSubtargetInfo(TT, CPU, /*TuneCPU=*/CPU, FS), InstrInfo(*this),
      FrameLowering(*this), TLInfo(TM, *this) {}
