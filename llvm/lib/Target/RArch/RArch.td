//===----------------------------------------------------------------------===//
// This is the top level entry point for the RArch target.
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Target-independent interfaces
//===----------------------------------------------------------------------===//

include "llvm/Target/Target.td"

//===----------------------------------------------------------------------===//
// RArch Subtarget features
//===----------------------------------------------------------------------===//

defvar GRV64 = DefaultMode;

//===----------------------------------------------------------------------===//
// Target-dependent interfaces
//===----------------------------------------------------------------------===//

include "RArchRegisterInfo.td"

//===----------------------------------------------------------------------===//
// Calling Conv, Instruction Descriptions
//===----------------------------------------------------------------------===//

include "RArchSchedule.td"
include "RArchCallingConv.td"

include "RArchInstrsInfo.td"
def RArchInstrInfo : InstrInfo;

//===----------------------------------------------------------------------===//
// RArch processors supported.
//===----------------------------------------------------------------------===//

class Proc<string Name, list<SubtargetFeature> Features>
 : Processor<Name, NoItineraries, Features>;

def : Proc<"generic", []>;

def RArchAsmParser : AsmParser {
  let ShouldEmitMatchRegisterAltName = 1;
  let AllowDuplicateRegisterNames = 1;
}

def RArch : Target {
  let InstructionSet = RArchInstrInfo;
}
