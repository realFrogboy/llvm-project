#ifndef __LLVM_LIB_TARGET_RArch_MCTARGETDESC_RArchMCASMINFO_H__
#define __LLVM_LIB_TARGET_RArch_MCTARGETDESC_RArchMCASMINFO_H__

#include "llvm/MC/MCAsmInfoELF.h"

namespace llvm {

class Triple;

class RArchMCAsmInfo : public MCAsmInfoELF {
  void anchor() override;

public:
  explicit RArchMCAsmInfo(const Triple &TT);
};

} // end namespace llvm

#endif // __LLVM_LIB_TARGET_RArch_MCTARGETDESC_RArchMCASMINFO_H__
