//===-- RArchELFStreamer.cpp - RArch ELF Target Streamer Methods ----------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file provides RArch specific target streamer methods.
//
//===----------------------------------------------------------------------===//

#include "RArchElfStreamer.h"
#include "RArchAsmBackend.h"
#include "RArchInfo.h"
#include "RArchMCTargetDesc.h"
#include "llvm/BinaryFormat/ELF.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCSectionELF.h"
#include "llvm/MC/MCValue.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/Support/LEB128.h"
#include "llvm/Support/Casting.h"

using namespace llvm;

// This part is for ELF object output.
RArchTargetELFStreamer::RArchTargetELFStreamer(MCStreamer &S,
                                                 const MCSubtargetInfo &STI)
    : RArchTargetStreamer(S), CurrentVendor("rarch") {
  MCAssembler &MCA = getStreamer().getAssembler();
  auto &MAB = static_cast<RArchAsmBackend &>(MCA.getBackend());
  rarchABI::ABI ABI = MAB.getTargetABI();
  switch (ABI) {
  case rarchABI::ABI_LP64:
    break;
  case rarchABI::ABI_Unknown:
    llvm_unreachable("Improperly initialised target ABI");
  }
}

MCELFStreamer &RArchTargetELFStreamer::getStreamer() {
  return static_cast<MCELFStreamer &>(Streamer);
}

void RArchTargetELFStreamer::emitDirectiveOptionPush() {}
void RArchTargetELFStreamer::emitDirectiveOptionPop() {}
void RArchTargetELFStreamer::emitDirectiveOptionPIC() {}
void RArchTargetELFStreamer::emitDirectiveOptionNoPIC() {}
void RArchTargetELFStreamer::emitDirectiveOptionRelax() {}
void RArchTargetELFStreamer::emitDirectiveOptionNoRelax() {}

void RArchTargetELFStreamer::emitAttribute(unsigned Attribute, unsigned Value) {
  setAttributeItem(Attribute, Value, /*OverwriteExisting=*/true);
}

void RArchTargetELFStreamer::emitTextAttribute(unsigned Attribute,
                                               StringRef String) {
  setAttributeItem(Attribute, String, /*OverwriteExisting=*/true);
}

void RArchTargetELFStreamer::emitIntTextAttribute(unsigned Attribute,
                                                  unsigned IntValue,
                                                  StringRef StringValue) {
  setAttributeItems(Attribute, IntValue, StringValue,
                    /*OverwriteExisting=*/true);
}

void RArchTargetELFStreamer::finishAttributeSection() {
  if (Contents.empty())
    return;

  if (AttributeSection) {
    Streamer.switchSection(AttributeSection);
  } else {
    MCAssembler &MCA = getStreamer().getAssembler();
    AttributeSection = MCA.getContext().getELFSection(
        ".RArch.attributes", ELF::SHT_RArch_ATTRIBUTES, 0);
    Streamer.switchSection(AttributeSection);

    Streamer.emitInt8(ELFAttrs::Format_Version);
  }

  // Vendor size + Vendor name + '\0'
  const size_t VendorHeaderSize = 4 + CurrentVendor.size() + 1;

  // Tag + Tag Size
  const size_t TagHeaderSize = 1 + 4;

  const size_t ContentsSize = calculateContentSize();

  Streamer.emitInt32(VendorHeaderSize + TagHeaderSize + ContentsSize);
  Streamer.emitBytes(CurrentVendor);
  Streamer.emitInt8(0); // '\0'

  Streamer.emitInt8(ELFAttrs::File);
  Streamer.emitInt32(TagHeaderSize + ContentsSize);

  // Size should have been accounted for already, now
  // emit each field as its type (ULEB or String).
  for (AttributeItem item : Contents) {
    Streamer.emitULEB128IntValue(item.Tag);
    switch (item.Type) {
    default:
      llvm_unreachable("Invalid attribute type");
    case AttributeType::Numeric:
      Streamer.emitULEB128IntValue(item.IntValue);
      break;
    case AttributeType::Text:
      Streamer.emitBytes(item.StringValue);
      Streamer.emitInt8(0); // '\0'
      break;
    case AttributeType::NumericAndText:
      Streamer.emitULEB128IntValue(item.IntValue);
      Streamer.emitBytes(item.StringValue);
      Streamer.emitInt8(0); // '\0'
      break;
    }
  }

  Contents.clear();
}

size_t RArchTargetELFStreamer::calculateContentSize() const {
  size_t Result = 0;
  for (AttributeItem item : Contents) {
    switch (item.Type) {
    case AttributeType::Hidden:
      break;
    case AttributeType::Numeric:
      Result += getULEB128Size(item.Tag);
      Result += getULEB128Size(item.IntValue);
      break;
    case AttributeType::Text:
      Result += getULEB128Size(item.Tag);
      Result += item.StringValue.size() + 1; // string + '\0'
      break;
    case AttributeType::NumericAndText:
      Result += getULEB128Size(item.Tag);
      Result += getULEB128Size(item.IntValue);
      Result += item.StringValue.size() + 1; // string + '\0';
      break;
    }
  }
  return Result;
}

namespace {
class RArchELFStreamer : public MCELFStreamer {
  static std::pair<unsigned, unsigned> getRelocPairForSize(unsigned Size) {
    switch (Size) {
    default:
      llvm_unreachable("unsupported fixup size");
    case 1:
      return std::make_pair(rarch::fixup_RArch_add_8, rarch::fixup_RArch_sub_8);
    case 2:
      return std::make_pair(rarch::fixup_RArch_add_16,
                            rarch::fixup_RArch_sub_16);
    case 4:
      return std::make_pair(rarch::fixup_RArch_add_32,
                            rarch::fixup_RArch_sub_32);
    case 8:
      return std::make_pair(rarch::fixup_RArch_add_64,
                            rarch::fixup_RArch_sub_64);
    }
  }

  static bool requiresFixups(MCContext &C, const MCExpr *Value,
                             const MCExpr *&LHS, const MCExpr *&RHS) {
    const auto *MBE = dyn_cast<MCBinaryExpr>(Value);
    if (MBE == nullptr)
      return false;

    MCValue E;
    if (!Value->evaluateAsRelocatable(E, nullptr, nullptr))
      return false;
    if (E.getSymA() == nullptr || E.getSymB() == nullptr)
      return false;

    const auto &A = E.getSymA()->getSymbol();
    const auto &B = E.getSymB()->getSymbol();

    LHS =
        MCBinaryExpr::create(MCBinaryExpr::Add, MCSymbolRefExpr::create(&A, C),
                             MCConstantExpr::create(E.getConstant(), C), C);
    RHS = E.getSymB();

    return (A.isInSection() ? A.getSection().hasInstructions()
                            : !A.getName().empty()) ||
           (B.isInSection() ? B.getSection().hasInstructions()
                            : !B.getName().empty());
  }

public:
  RArchELFStreamer(MCContext &C, std::unique_ptr<MCAsmBackend> MAB,
                    std::unique_ptr<MCObjectWriter> MOW,
                    std::unique_ptr<MCCodeEmitter> MCE)
      : MCELFStreamer(C, std::move(MAB), std::move(MOW), std::move(MCE)) {}

  void emitValueImpl(const MCExpr *Value, unsigned Size, SMLoc Loc) override {
    const MCExpr *A, *B;
    if (!requiresFixups(getContext(), Value, A, B))
      return MCELFStreamer::emitValueImpl(Value, Size, Loc);

    MCStreamer::emitValueImpl(Value, Size, Loc);

    MCDataFragment *DF = getOrCreateDataFragment();
    MCDwarfLineEntry::make(this, getCurrentSectionOnly());

    unsigned Add, Sub;
    std::tie(Add, Sub) = getRelocPairForSize(Size);

    DF->getFixups().push_back(MCFixup::create(
        DF->getContents().size(), A, static_cast<MCFixupKind>(Add), Loc));
    DF->getFixups().push_back(MCFixup::create(
        DF->getContents().size(), B, static_cast<MCFixupKind>(Sub), Loc));

    DF->getContents().resize(DF->getContents().size() + Size, 0);
  }
};
} // namespace

namespace llvm {
MCELFStreamer *createRArchELFStreamer(MCContext &C,
                                       std::unique_ptr<MCAsmBackend> MAB,
                                       std::unique_ptr<MCObjectWriter> MOW,
                                       std::unique_ptr<MCCodeEmitter> MCE) {
  RArchELFStreamer *S =
      new RArchELFStreamer(C, std::move(MAB), std::move(MOW), std::move(MCE));
  return S;
}
} // namespace llvm
