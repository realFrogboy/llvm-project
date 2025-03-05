add_llvm_component_library(LLVMRArchInfo
  RArchTargetInfo.cpp

  LINK_COMPONENTS
  MC
  Support

  ADD_TO_COMPONENT
  RArch
  )
