//===-- RArchTargetInfo.cpp - RArch Target Implementation ----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===---------------------------------------------------------------------===//

#include "TargetInfo/RArchTargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
using namespace llvm;

Target &llvm::getTheRArchTarget() {
  static Target TheRArchTarget;
  return TheRArchTarget;
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeRArchTargetInfo() {
  RegisterTarget<Triple::rarch, /*HasJIT=*/false> X(
        getTheRArchTarget(), "rarch", "64-bit RArch", "RArch");
}
