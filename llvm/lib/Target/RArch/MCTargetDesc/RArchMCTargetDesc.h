//===-- RArchMCTargetDesc.h - RArch Target Descriptions --------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file provides RArch specific target descriptions.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_RArch_MCTARGETDESC_RArchMCTARGETDESC_H
#define LLVM_LIB_TARGET_RArch_MCTARGETDESC_RArchMCTARGETDESC_H

#include "llvm/MC/MCTargetOptions.h"
#include <memory>

namespace llvm {
class Triple;
class MCAsmBackend;
class MCCodeEmitter;
class MCContext;
class MCInstrInfo;
class MCObjectTargetWriter;
class MCRegisterInfo;
class MCSubtargetInfo;
class Target;

extern Target TheRArchTarget;

MCCodeEmitter *createRArchMCCodeEmitter(const MCInstrInfo &MCII,
                                         MCContext &Ctx);

std::unique_ptr<MCObjectTargetWriter> createRArchELFObjectWriter(uint8_t OSABI,
                                                                  bool Is64Bit);

MCAsmBackend *createRArchAsmBackend(const Target &T, const MCSubtargetInfo &STI,
                                     const MCRegisterInfo &MRI,
                                     const MCTargetOptions &Options);
} // namespace llvm

#endif // LLVM_LIB_TARGET_RArch_MCTARGETDESC_RArchMCTARGETDESC_H
