add_llvm_component_library(LLVMRArchDesc
  RArchMCTargetDesc.cpp
  RArchInfo.cpp

  RArchMCAsmInfo.cpp
  RArchMCExpr.cpp
  RArchObjectFileInfo.cpp
  RArchInstPrinter.cpp
  RArchTargetStreamer.cpp
  RArchAsmBackend.cpp
  RArchMCCodeEmitter.cpp
  RArchELFObjectWriter.cpp
  RArchElfStreamer.cpp

  LINK_COMPONENTS
  RArchInfo
  MC
  Support

  ADD_TO_COMPONENT
  RArch
  )
