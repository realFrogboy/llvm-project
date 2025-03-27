//===-- RArchMCObjectFileInfo.cpp - rarch object file properties ----------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the declarations of the RArchMCObjectFileInfo properties.
//
//===----------------------------------------------------------------------===//

#include "RArchObjectFileInfo.h"

using namespace llvm;

unsigned RArchMCObjectFileInfo::getTextSectionAlignment() const {
  return 4;
}
