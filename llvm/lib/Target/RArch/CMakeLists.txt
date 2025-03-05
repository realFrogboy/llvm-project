add_llvm_component_group(RArch)

add_llvm_target(RArchCodeGen
  RArchTargetMachine.cpp

  LINK_COMPONENTS
  Analysis
  AsmPrinter
  CodeGen
  CodeGenTypes
  Core
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
