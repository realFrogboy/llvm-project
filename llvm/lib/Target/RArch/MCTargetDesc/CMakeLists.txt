add_llvm_component_library(LLVMRArchDesc
  RArchMCTargetDesc.cpp

  LINK_COMPONENTS
  RArchInfo
  MC
  Support

  ADD_TO_COMPONENT
  RArch
  )
