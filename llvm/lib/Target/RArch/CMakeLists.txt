add_llvm_component_group(RArch)

set(LLVM_TARGET_DEFINITIONS RArch.td)

tablegen(LLVM RArchGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM RArchGenInstrsInfo.inc -gen-instr-info)
tablegen(LLVM RArchGenCallingConv.inc -gen-callingconv)
tablegen(LLVM RArchGenSubtargetInfo.inc -gen-subtarget)
tablegen(LLVM RArchGenMCPseudoLowering.inc -gen-pseudo-lowering)
tablegen(LLVM RArchGenDAGISel.inc -gen-dag-isel)
tablegen(LLVM RArchGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM RArchGenMCCodeEmitter.inc -gen-emitter)

add_public_tablegen_target(RArchCommonTableGen)

add_llvm_target(RArchCodeGen
  RArchTargetMachine.cpp
  RArchSubtarget.cpp

  RArchRegisterInfo.cpp
  RArchInstrsInfo.cpp

  RArchISelLowering.cpp
  RArchFrameLowering.cpp

  RArchMatInt.cpp
  RArchISelDagToDag.cpp
  RArchAsmPrinter.cpp

  LINK_COMPONENTS
  Analysis
  AsmPrinter
  CodeGen
  CodeGenTypes
  Core
  MC
  GlobalISel
  RArchInfo
  Scalar
  SelectionDAG
  Support
  Target
  TargetParser

  ADD_TO_COMPONENT
  RArch
  )

add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)
