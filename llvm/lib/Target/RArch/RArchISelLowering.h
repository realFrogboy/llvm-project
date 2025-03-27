#ifndef __LLVM_LIB_TARGET_RArch_RArchISELLOWERING_H__
#define __LLVM_LIB_TARGET_RArch_RArchISELLOWERING_H__

#include "MCTargetDesc/RArchInfo.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/TargetLowering.h"

namespace llvm {

namespace rarchISD {

enum NodeType : unsigned {
  // Start the numbering where the builtin ops and target ops leave off.
  FIRST_NUMBER = ISD::BUILTIN_OP_END,
  RET_FLAG,
  BR_CC,
  CALL,

  DIVW,
  DIVUW,
  REMUW,
};

} // namespace rarchISD

class RArchTargetLowering : public TargetLowering {
public:
  explicit RArchTargetLowering(const TargetMachine &TM,
                                const RArchSubtarget &STI);

  /// Provide custom lowering hooks for some operations.
  SDValue LowerOperation(SDValue Op, SelectionDAG &DAG) const override;

  /// This method returns the name of a target specific DAG node.
  const char *getTargetNodeName(unsigned Opcode) const override;

  /// Return true if the addressing mode represented by AM is legal for this
  /// target, for a load/store of the specified type.
  bool isLegalAddressingMode(const DataLayout &DL, const AddrMode &AM, Type *Ty,
                             unsigned AS,
                             Instruction *I = nullptr) const override;

  RArchSubtarget const &getSubtarget() const { return Subtarget; }

private:
  const RArchSubtarget &Subtarget;

  /// RArchCCAssignFn - This target-specific function extends the default
  /// CCValAssign with additional information used to lower RArch calling
  /// conventions.
  typedef bool RArchCCAssignFn(const DataLayout &DL, rarchABI::ABI,
                                unsigned ValNo, MVT ValVT, MVT LocVT,
                                CCValAssign::LocInfo LocInfo,
                                ISD::ArgFlagsTy ArgFlags, CCState &State,
                                bool IsFixed, bool IsRet, Type *OrigTy,
                                const RArchTargetLowering &TLI,
                                std::optional<unsigned> FirstMaskArgument);

  void analyzeInputArgs(MachineFunction &MF, CCState &CCInfo,
                        const SmallVectorImpl<ISD::InputArg> &Ins, bool IsRet,
                        RArchCCAssignFn Fn) const;
  void analyzeOutputArgs(MachineFunction &MF, CCState &CCInfo,
                         const SmallVectorImpl<ISD::OutputArg> &Outs,
                         bool IsRet, CallLoweringInfo *CLI,
                         RArchCCAssignFn Fn) const;

  SDValue PerformDAGCombine(SDNode *N, DAGCombinerInfo &DCI) const override;

  SDValue LowerFormalArguments(SDValue Chain, CallingConv::ID CallConv,
                               bool IsVarArg,
                               const SmallVectorImpl<ISD::InputArg> &Ins,
                               const SDLoc &DL, SelectionDAG &DAG,
                               SmallVectorImpl<SDValue> &InVals) const override;

  SDValue LowerCall(TargetLowering::CallLoweringInfo &CLI,
                    SmallVectorImpl<SDValue> &InVals) const override;

  SDValue LowerReturn(SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
                      const SmallVectorImpl<ISD::OutputArg> &Outs,
                      const SmallVectorImpl<SDValue> &OutVals, const SDLoc &DL,
                      SelectionDAG &DAG) const override;

  bool CanLowerReturn(CallingConv::ID CallConv, MachineFunction &MF,
                      bool IsVarArg,
                      const SmallVectorImpl<ISD::OutputArg> &ArgsFlags,
                      LLVMContext &Context, const Type *RetTy) const override;

  bool mayBeEmittedAsTailCall(const CallInst *CI) const override;

  SDValue lowerBR_CC(SDValue Op, SelectionDAG &DAG) const;
  SDValue lowerFRAMEADDR(SDValue Op, SelectionDAG &DAG) const;
};

} // end namespace llvm

#endif // __LLVM_LIB_TARGET_RArch_RArchISELLOWERING_H__
