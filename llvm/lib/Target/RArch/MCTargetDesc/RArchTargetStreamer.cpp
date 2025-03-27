//===-- RArchTargetStreamer.cpp - rarch Target Streamer Methods -----------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file provides rarch specific target streamer methods.
//
//===----------------------------------------------------------------------===//

#include "RArchInfo.h"
#include "RArchTargetStreamer.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/RArchAttributes.h"
#include "llvm/Support/RArchISAInfo.h"

using namespace llvm;

RArchTargetStreamer::RArchTargetStreamer(MCStreamer &S) : MCTargetStreamer(S) {}

void RArchTargetStreamer::finish() { finishAttributeSection(); }

void RArchTargetStreamer::emitDirectiveOptionPush() {}
void RArchTargetStreamer::emitDirectiveOptionPop() {}
void RArchTargetStreamer::emitDirectiveOptionPIC() {}
void RArchTargetStreamer::emitDirectiveOptionNoPIC() {}
void RArchTargetStreamer::emitDirectiveOptionRelax() {}
void RArchTargetStreamer::emitDirectiveOptionNoRelax() {}
void RArchTargetStreamer::emitAttribute(unsigned Attribute, unsigned Value) {}
void RArchTargetStreamer::finishAttributeSection() {}
void RArchTargetStreamer::emitTextAttribute(unsigned Attribute,
                                             StringRef String) {}
void RArchTargetStreamer::emitIntTextAttribute(unsigned Attribute,
                                                unsigned IntValue,
                                                StringRef StringValue) {}

void RArchTargetStreamer::emitTargetAttributes(const MCSubtargetInfo &STI) {
  emitAttribute(RArchAttrs::STACK_ALIGN, RArchAttrs::ALIGN_16);

  unsigned XLen = 64;
  std::vector<std::string> FeatureVector;
  rarchFeatures::toFeatureVector(FeatureVector, STI.getFeatureBits());

  auto ParseResult = llvm::RArchISAInfo::parseFeatures(XLen, FeatureVector);
  if (!ParseResult) {
    /* Assume any error about features should handled earlier.  */
    consumeError(ParseResult.takeError());
    llvm_unreachable("Parsing feature error when emitTargetAttributes?");
  } else {
    auto &ISAInfo = *ParseResult;
    emitTextAttribute(RArchAttrs::ARCH, ISAInfo->toString());
  }
}

// This part is for ascii assembly output
RArchTargetAsmStreamer::RArchTargetAsmStreamer(MCStreamer &S,
                                                 formatted_raw_ostream &OS)
    : RArchTargetStreamer(S), OS(OS) {}

void RArchTargetAsmStreamer::emitDirectiveOptionPush() {
  OS << "\t.option\tpush\n";
}

void RArchTargetAsmStreamer::emitDirectiveOptionPop() {
  OS << "\t.option\tpop\n";
}

void RArchTargetAsmStreamer::emitDirectiveOptionPIC() {
  OS << "\t.option\tpic\n";
}

void RArchTargetAsmStreamer::emitDirectiveOptionNoPIC() {
  OS << "\t.option\tnopic\n";
}

void RArchTargetAsmStreamer::emitDirectiveOptionRelax() {
  OS << "\t.option\trelax\n";
}

void RArchTargetAsmStreamer::emitDirectiveOptionNoRelax() {
  OS << "\t.option\tnorelax\n";
}

void RArchTargetAsmStreamer::emitAttribute(unsigned Attribute, unsigned Value) {
  OS << "\t.attribute\t" << Attribute << ", " << Twine(Value) << "\n";
}

void RArchTargetAsmStreamer::emitTextAttribute(unsigned Attribute,
                                                StringRef String) {
  OS << "\t.attribute\t" << Attribute << ", \"" << String << "\"\n";
}

void RArchTargetAsmStreamer::emitIntTextAttribute(unsigned Attribute,
                                                   unsigned IntValue,
                                                   StringRef StringValue) {}

void RArchTargetAsmStreamer::finishAttributeSection() {}
