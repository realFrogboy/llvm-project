//===-- RArchMCObjectFileInfo.h - rarch object file Info -------*- C++ -*--===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the RArchMCObjectFileInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_RArch_MCTARGETDESC_RArchMCOBJECTFILEINFO_H
#define LLVM_LIB_TARGET_RArch_MCTARGETDESC_RArchMCOBJECTFILEINFO_H

#include "llvm/MC/MCObjectFileInfo.h"

namespace llvm {

class RArchMCObjectFileInfo : public MCObjectFileInfo {
public:
  unsigned getTextSectionAlignment() const override;
};

} // namespace llvm

#endif // LLVM_LIB_TARGET_RArch_MCTARGETDESC_RArchMCOBJECTFILEINFO_H
