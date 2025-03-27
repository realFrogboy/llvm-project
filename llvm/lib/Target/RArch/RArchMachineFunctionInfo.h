//===----------------------------------------------------------------------===//
//
// This file declares rarch-specific per-machine-function information.
//
//===----------------------------------------------------------------------===//

#ifndef __LLVM_LIB_TARGET_RArch_RArchMACHINEFUNCTIONINFO_H__
#define __LLVM_LIB_TARGET_RArch_RArchMACHINEFUNCTIONINFO_H__

#include "llvm/CodeGen/MachineFunction.h"

namespace llvm {

class RArchFunctionInfo : public MachineFunctionInfo {
  virtual void anchor() {}

  bool ReturnStackOffsetSet = false;
  unsigned ReturnStackOffset = -1U;

  int VarArgsFrameIndex = 0;
  int VarArgsSaveSize = 0;

  /// Size of stack frame to save callee saved registers
  unsigned CalleeSavedStackSize = 0;

public:
  RArchFunctionInfo() {}
  explicit RArchFunctionInfo(const Function &F, const TargetSubtargetInfo *STI) {}

  void setVarArgsFrameIndex(int Off) { VarArgsFrameIndex = Off; }
  int getVarArgsFrameIndex() const { return VarArgsFrameIndex; }

  void setVarArgsSaveSize(int Size) { VarArgsSaveSize = Size; }
  int getVarArgsSaveSize() const { return VarArgsSaveSize; }

  void setCalleeSavedStackSize(unsigned Size) { CalleeSavedStackSize = Size; }
  unsigned getCalleeSavedStackSize() const { return CalleeSavedStackSize; }

  void setReturnStackOffset(unsigned Off) {
    assert(!ReturnStackOffsetSet && "Return stack offset set twice");
    ReturnStackOffset = Off;
    ReturnStackOffsetSet = true;
  }

  unsigned getReturnStackOffset() const {
    assert(ReturnStackOffsetSet && "Return stack offset not set");
    return ReturnStackOffset;
  }
};

} // end namespace llvm

#endif // __LLVM_LIB_TARGET_RArch_RArchMACHINEFUNCTIONINFO_H__
