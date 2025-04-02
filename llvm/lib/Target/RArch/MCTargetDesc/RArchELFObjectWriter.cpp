//===-- RArchELFObjectWriter.cpp - RArch ELF Writer -----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/RArchFixupKinds.h"
#include "MCTargetDesc/RArchMCExpr.h"
#include "MCTargetDesc/RArchMCTargetDesc.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

namespace {
class RArchELFObjectWriter : public MCELFObjectTargetWriter {
public:
  RArchELFObjectWriter(uint8_t OSABI, bool Is64Bit);

  ~RArchELFObjectWriter() override;

  // Return true if the given relocation must be with a symbol rather than
  // section plus offset.
  bool needsRelocateWithSymbol(const MCValue &Val, const MCSymbol &Sym,
                               unsigned Type) const override {
    return true;
  }

protected:
  unsigned getRelocType(MCContext &Ctx, const MCValue &Target,
                        const MCFixup &Fixup, bool IsPCRel) const override;
};
}

RArchELFObjectWriter::RArchELFObjectWriter(uint8_t OSABI, bool Is64Bit)
    : MCELFObjectTargetWriter(Is64Bit, OSABI, ELF::EM_RArch,
                              /*HasRelocationAddend*/ true) {}

RArchELFObjectWriter::~RArchELFObjectWriter() {}

unsigned RArchELFObjectWriter::getRelocType(MCContext &Ctx,
                                             const MCValue &Target,
                                             const MCFixup &Fixup,
                                             bool IsPCRel) const {
  const MCExpr *Expr = Fixup.getValue();
  // Determine the type of the relocation
  unsigned Kind = Fixup.getTargetKind();
  if (Kind >= FirstLiteralRelocationKind)
    return Kind - FirstLiteralRelocationKind;
  if (IsPCRel) {
    switch (Kind) {
    default:
      Ctx.reportError(Fixup.getLoc(), "Unsupported relocation type");
      return ELF::R_RArch_NONE;
    case FK_Data_4:
    case FK_PCRel_4:
      return ELF::R_RArch_32_PCREL;
    case rarch::fixup_RArch_pcrel_hi20:
      return ELF::R_RArch_PCREL_HI20;
    case rarch::fixup_RArch_pcrel_lo12_i:
      return ELF::R_RArch_PCREL_LO12_I;
    case rarch::fixup_RArch_pcrel_lo12_s:
      return ELF::R_RArch_PCREL_LO12_S;
    case rarch::fixup_RArch_got_hi20:
      return ELF::R_RArch_GOT_HI20;
    case rarch::fixup_RArch_tls_got_hi20:
      return ELF::R_RArch_TLS_GOT_HI20;
    case rarch::fixup_RArch_tls_gd_hi20:
      return ELF::R_RArch_TLS_GD_HI20;
    case rarch::fixup_RArch_jal:
      return ELF::R_RArch_JAL;
    case rarch::fixup_RArch_branch:
      return ELF::R_RArch_BRANCH;
    case rarch::fixup_RArch_call:
      return ELF::R_RArch_CALL;
    case rarch::fixup_RArch_call_plt:
      return ELF::R_RArch_CALL_PLT;
    case rarch::fixup_RArch_add_8:
      return ELF::R_RArch_ADD8;
    case rarch::fixup_RArch_sub_8:
      return ELF::R_RArch_SUB8;
    case rarch::fixup_RArch_add_16:
      return ELF::R_RArch_ADD16;
    case rarch::fixup_RArch_sub_16:
      return ELF::R_RArch_SUB16;
    case rarch::fixup_RArch_add_32:
      return ELF::R_RArch_ADD32;
    case rarch::fixup_RArch_sub_32:
      return ELF::R_RArch_SUB32;
    case rarch::fixup_RArch_add_64:
      return ELF::R_RArch_ADD64;
    case rarch::fixup_RArch_sub_64:
      return ELF::R_RArch_SUB64;
    }
  }

  switch (Kind) {
  default:
    Ctx.reportError(Fixup.getLoc(), "Unsupported relocation type");
    return ELF::R_RArch_NONE;
  case FK_Data_1:
    Ctx.reportError(Fixup.getLoc(), "1-byte data relocations not supported");
    return ELF::R_RArch_NONE;
  case FK_Data_2:
    Ctx.reportError(Fixup.getLoc(), "2-byte data relocations not supported");
    return ELF::R_RArch_NONE;
  case FK_Data_4:
    if (Expr->getKind() == MCExpr::Target &&
        cast<RArchMCExpr>(Expr)->getKind() == RArchMCExpr::VK_RArch_32_PCREL)
      return ELF::R_RArch_32_PCREL;
    return ELF::R_RArch_32;
  case FK_Data_8:
    return ELF::R_RArch_64;
  case rarch::fixup_RArch_hi20:
    return ELF::R_RArch_HI20;
  case rarch::fixup_RArch_lo12_i:
    return ELF::R_RArch_LO12_I;
  case rarch::fixup_RArch_lo12_s:
    return ELF::R_RArch_LO12_S;
  case rarch::fixup_RArch_tprel_hi20:
    return ELF::R_RArch_TPREL_HI20;
  case rarch::fixup_RArch_tprel_lo12_i:
    return ELF::R_RArch_TPREL_LO12_I;
  case rarch::fixup_RArch_tprel_lo12_s:
    return ELF::R_RArch_TPREL_LO12_S;
  case rarch::fixup_RArch_tprel_add:
    return ELF::R_RArch_TPREL_ADD;
  case rarch::fixup_RArch_relax:
    return ELF::R_RArch_RELAX;
  case rarch::fixup_RArch_align:
    return ELF::R_RArch_ALIGN;
  case rarch::fixup_RArch_set_6b:
    return ELF::R_RArch_SET6;
  case rarch::fixup_RArch_sub_6b:
    return ELF::R_RArch_SUB6;
  case rarch::fixup_RArch_add_8:
    return ELF::R_RArch_ADD8;
  case rarch::fixup_RArch_set_8:
    return ELF::R_RArch_SET8;
  case rarch::fixup_RArch_sub_8:
    return ELF::R_RArch_SUB8;
  case rarch::fixup_RArch_set_16:
    return ELF::R_RArch_SET16;
  case rarch::fixup_RArch_add_16:
    return ELF::R_RArch_ADD16;
  case rarch::fixup_RArch_sub_16:
    return ELF::R_RArch_SUB16;
  case rarch::fixup_RArch_set_32:
    return ELF::R_RArch_SET32;
  case rarch::fixup_RArch_add_32:
    return ELF::R_RArch_ADD32;
  case rarch::fixup_RArch_sub_32:
    return ELF::R_RArch_SUB32;
  case rarch::fixup_RArch_add_64:
    return ELF::R_RArch_ADD64;
  case rarch::fixup_RArch_sub_64:
    return ELF::R_RArch_SUB64;
  }
}

std::unique_ptr<MCObjectTargetWriter>
llvm::createRArchELFObjectWriter(uint8_t OSABI, bool Is64Bit) {
  return std::make_unique<RArchELFObjectWriter>(OSABI, Is64Bit);
}
