//===-- RArchInstrFormats.td - RArch Instruction Formats --*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// Generic rarch Format
class InstFormat<bits<5> val> {
  bits<5> Value = val;
}

// From RISC-V-like unprivileged ISA
def InstFormatPseudo      : InstFormat<0>;
def InstFormatR           : InstFormat<1>;
def InstFormatI           : InstFormat<2>;
def InstFormatS           : InstFormat<3>;
def InstFormatB           : InstFormat<4>;
def InstFormatU           : InstFormat<5>;
def InstFormatJ           : InstFormat<6>;
def InstFormatIntr        : InstFormat<7>;

class rarchOpcode<string name, bits<7> val> {
  string Name = name;
  bits<7> Value = val;
}

// TODO: delete commented instruction type if they are innecessary
def OPC_LOAD      : rarchOpcode<"LOAD",      0b0000011>;
// def OPC_LOAD_FP   : rarchOpcode<"LOAD_FP",   0b0000111>;
def OPC_MISC_MEM  : rarchOpcode<"MISC_MEM",  0b0001111>;
def OPC_OP_IMM    : rarchOpcode<"OP_IMM",    0b0010011>;
def OPC_AUIPC     : rarchOpcode<"AUIPC",     0b0010111>;
def OPC_OP_IMM_32 : rarchOpcode<"OP_IMM_32", 0b0011011>;
def OPC_STORE     : rarchOpcode<"STORE",     0b0100011>;
// def OPC_STORE_FP  : rarchOpcode<"STORE_FP",  0b0100111>;
// def OPC_AMO       : rarchOpcode<"AMO",       0b0101111>;
def OPC_OP        : rarchOpcode<"OP",        0b0110011>;
def OPC_LUI       : rarchOpcode<"LUI",       0b0110111>;
def OPC_OP_32     : rarchOpcode<"OP_32",     0b0111011>;
// def OPC_MADD      : rarchOpcode<"MADD",      0b1000011>;
// def OPC_MSUB      : rarchOpcode<"MSUB",      0b1000111>;
// def OPC_NMSUB     : rarchOpcode<"NMSUB",     0b1001011>;
// def OPC_NMADD     : rarchOpcode<"NMADD",     0b1001111>;
// def OPC_OP_FP     : rarchOpcode<"OP_FP",     0b1010011>;
// def OPC_OP_V      : rarchOpcode<"OP_V",      0b1010111>;
def OPC_BRANCH    : rarchOpcode<"BRANCH",    0b1100011>;
def OPC_JALR      : rarchOpcode<"JALR",      0b1100111>;
def OPC_JAL       : rarchOpcode<"JAL",       0b1101111>;
def OPC_SYSTEM    : rarchOpcode<"SYSTEM",    0b1110011>;
def OPC_INTRINSIC : rarchOpcode<"INTRINSIC", 0b1111111>;

class rarchInst<dag outs, dag ins, string opcodestr, string argstr,
             list<dag> pattern, InstFormat format>
    : Instruction {
  let Namespace = "rarch";
  let DecoderNamespace = "rarch";
  let Size = 4;

  field bits<32> Inst;

  bits<7> Opcode = 0;

  let Inst{6-0} = Opcode;

  dag OutOperandList = outs;
  dag InOperandList = ins;
  let AsmString = opcodestr # "\t" # argstr;
  let Pattern = pattern;

  let TSFlags{4-0} = format.Value;
}

// Pseudo instructions
class Pseudo<dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">
    : rarchInst<outs, ins, opcodestr, argstr, pattern, InstFormatPseudo> {
  let isPseudo = 1;
  let isCodeGenOnly = 1;
}

// Pseudo load instructions.
class PseudoLoad<string opcodestr, RegisterClass rdty = GPR>
    : Pseudo<(outs rdty:$rd), (ins bare_symbol:$addr), [], opcodestr, "$rd, $addr"> {
  let hasSideEffects = 0;
  let mayLoad = 1;
  let mayStore = 0;
  let isCodeGenOnly = 0;
  let isAsmParserOnly = 1;
}

// Pseudo store instructions.
class PseudoStore<string opcodestr, RegisterClass rsty = GPR>
    : Pseudo<(outs GPR:$tmp), (ins rsty:$rs, bare_symbol:$addr), [], opcodestr, "$rs, $addr, $tmp"> {
  let hasSideEffects = 0;
  let mayLoad = 0;
  let mayStore = 1;
  let isCodeGenOnly = 0;
  let isAsmParserOnly = 1;
}

class rarchInstR<bits<7> funct7, bits<3> funct3, rarchOpcode opcode, dag outs,
              dag ins, string opcodestr, string argstr>
    : rarchInst<outs, ins, opcodestr, argstr, [], InstFormatR> {
  bits<5> rs2;
  bits<5> rs1;
  bits<5> rd;

  let Inst{31-25} = funct7;
  let Inst{24-20} = rs2;
  let Inst{19-15} = rs1;
  let Inst{14-12} = funct3;
  let Inst{11-7} = rd;
  let Opcode = opcode.Value;
}

class rarchInstIntr<bits<3> funct3, rarchOpcode opcode, dag outs,
              dag ins, string opcodestr, string argstr, list<dag> pattern>
    : rarchInst<outs, ins, opcodestr, argstr, pattern, InstFormatIntr> {
  bits<12> imm12;
  bits<5> rs1;
  bits<5> rd;

  let Inst{31-20} = imm12;
  let Inst{19-15} = rs1;
  let Inst{14-12} = funct3;
  let Inst{11-7} = rd;
  let Opcode = opcode.Value;
}

// class rarchInstIntV<bits<12> funct12, rarchOpcode opcode,
//           dag outs, dag ins, string opcodestr, string argstr, list<dag> pattern>
//     : rarchInst<outs, ins, opcodestr, argstr, pattern, InstFormatIntr> {
//   let Inst{31-20} = funct12;
//   let Inst{19-15} = 0;
//   let Inst{14-12} = 0;
//   let Inst{11-7} = 0;
//   let Opcode = opcode.Value;
// }

class rarchInstI<bits<3> funct3, rarchOpcode opcode, dag outs, dag ins,
              string opcodestr, string argstr>
    : rarchInst<outs, ins, opcodestr, argstr, [], InstFormatI> {
  bits<12> imm12;
  bits<5> rs1;
  bits<5> rd;

  let Inst{31-20} = imm12;
  let Inst{19-15} = rs1;
  let Inst{14-12} = funct3;
  let Inst{11-7} = rd;
  let Opcode = opcode.Value;
}

class rarchInstS<bits<3> funct3, rarchOpcode opcode, dag outs, dag ins,
              string opcodestr, string argstr>
    : rarchInst<outs, ins, opcodestr, argstr, [], InstFormatS> {
  bits<12> imm12;
  bits<5> rs2;
  bits<5> rs1;

  let Inst{31-25} = imm12{11-5};
  let Inst{24-20} = rs2;
  let Inst{19-15} = rs1;
  let Inst{14-12} = funct3;
  let Inst{11-7} = imm12{4-0};
  let Opcode = opcode.Value;
}

class rarchInstB<bits<3> funct3, rarchOpcode opcode, dag outs, dag ins,
              string opcodestr, string argstr>
    : rarchInst<outs, ins, opcodestr, argstr, [], InstFormatB> {
  bits<12> imm12;
  bits<5> rs2;
  bits<5> rs1;

  let Inst{31} = imm12{11};
  let Inst{30-25} = imm12{9-4};
  let Inst{24-20} = rs2;
  let Inst{19-15} = rs1;
  let Inst{14-12} = funct3;
  let Inst{11-8} = imm12{3-0};
  let Inst{7} = imm12{10};
  let Opcode = opcode.Value;
}

class rarchInstU<rarchOpcode opcode, dag outs, dag ins, string opcodestr,
              string argstr>
    : rarchInst<outs, ins, opcodestr, argstr, [], InstFormatU> {
  bits<20> imm20;
  bits<5> rd;

  let Inst{31-12} = imm20;
  let Inst{11-7} = rd;
  let Opcode = opcode.Value;
}

class rarchInstJ<rarchOpcode opcode, dag outs, dag ins, string opcodestr,
              string argstr>
    : rarchInst<outs, ins, opcodestr, argstr, [], InstFormatJ> {
  bits<20> imm20;
  bits<5> rd;

  let Inst{31} = imm20{19};
  let Inst{30-21} = imm20{9-0};
  let Inst{20} = imm20{10};
  let Inst{19-12} = imm20{18-11};
  let Inst{11-7} = rd;
  let Opcode = opcode.Value;
}

class rarchInstIShift<bits<5> imm11_7, bits<3> funct3, rarchOpcode opcode,
                   dag outs, dag ins, string opcodestr, string argstr>
    : rarchInst<outs, ins, opcodestr, argstr, [], InstFormatI> {
  bits<6> shamt;
  bits<5> rs1;
  bits<5> rd;

  let Inst{31-27} = imm11_7;
  let Inst{26} = 0;
  let Inst{25-20} = shamt;
  let Inst{19-15} = rs1;
  let Inst{14-12} = funct3;
  let Inst{11-7} = rd;
  let Opcode = opcode.Value;
}

class rarchInstIShiftW<bits<7> imm11_5, bits<3> funct3, rarchOpcode opcode,
                    dag outs, dag ins, string opcodestr, string argstr>
    : rarchInst<outs, ins, opcodestr, argstr, [], InstFormatI> {
  bits<5> shamt;
  bits<5> rs1;
  bits<5> rd;

  let Inst{31-25} = imm11_5;
  let Inst{24-20} = shamt;
  let Inst{19-15} = rs1;
  let Inst{14-12} = funct3;
  let Inst{11-7} = rd;
  let Opcode = opcode.Value;
}
