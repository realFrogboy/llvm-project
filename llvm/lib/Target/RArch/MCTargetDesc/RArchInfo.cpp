#include "RArchInfo.h"

#define GET_REGINFO_ENUM
#include "RArchGenRegisterInfo.inc"

namespace llvm {
  namespace rarchABI {
    MCRegister getBPReg() { return rarch::X9; }
    MCRegister getSCSPReg() { return rarch::X18; }
  } // namespace rarchABI
} // namespace llvm
