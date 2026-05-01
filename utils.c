#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

/* Sign extends the given field to a 32-bit integer where field is
 * interpreted an n-bit integer. */
void print_binary(unsigned int number, int size); // Size is in bits

int sign_extend_number(unsigned int field, unsigned int n) {
  /* YOUR CODE HERE */
  // first, shift the field to the end of the word, then use arithmetic shift to right
  //print_binary((int)(field << (32 - n)) >> (32 - n),32);
  //printf("--");
  return (int)(field << (32 - n)) >> (32 - n);
  
}

/* Unpacks the 32-bit machine code instruction given into the correct
 * type within the instruction struct */
Instruction parse_instruction(uint32_t instruction_bits) {
  /* YOUR CODE HERE */
  Instruction instruction;
  // add x8, x0, x0     hex : 00000433  binary = 0000 0000 0000 0000 0000 01000
  // Opcode: 0110011 (0x33) Get the Opcode by &ing 0x1111111, bottom 7 bits
  instruction.opcode = instruction_bits & ((1U << 7) - 1);
  // printf("{{==>}}");
  // print_binary(instruction.opcode,32);
  // printf("}}");

  // Shift right to move to pointer to interpret next fields in instruction.
  instruction_bits >>= 7;

  switch (instruction.opcode) {
    //lswc: opcode: 0x2a
    // opcode : 0x2a
    // type: R type
    // f3: 0x1
    // f7: 0x0
    // R-Type
    // lswc rd,rs1,rs2	R	0x2a	0x1	0x0	if (MEM[rs1] < rs2) MEM[rs1] = rs2, rd = MEM[rs1]
  case 0x2a:
    //printf("foundit\n");
  //0000000 00000 11111 001 01010 0101010
    // the same as r-type
    instruction.rtype.rd = instruction_bits & ((1U << 5) - 1);
    instruction_bits >>= 5;

    // instruction: 0000 0000 0000 0000 0 func3 : 000
    instruction.rtype.funct3 = instruction_bits & ((1U << 3) - 1);
    instruction_bits >>= 3;

    // instruction: 0000 0000 0000  src1: 00000
    instruction.rtype.rs1 = instruction_bits & ((1U << 5) - 1);
    instruction_bits >>= 5;

    // instruction: 0000 000        src2: 00000
    instruction.rtype.rs2 = instruction_bits & ((1U << 5) - 1);
    instruction_bits >>= 5;

    // funct7: 0000 000
    instruction.rtype.funct7 = instruction_bits & ((1U << 7) - 1);
    
    break;
  case 0x33:
    // instruction: 0000 0000 0000 0000 0000 destination : 01000
    instruction.rtype.rd = instruction_bits & ((1U << 5) - 1);
    instruction_bits >>= 5;

    // instruction: 0000 0000 0000 0000 0 func3 : 000
    instruction.rtype.funct3 = instruction_bits & ((1U << 3) - 1);
    instruction_bits >>= 3;

    // instruction: 0000 0000 0000  src1: 00000
    instruction.rtype.rs1 = instruction_bits & ((1U << 5) - 1);
    instruction_bits >>= 5;

    // instruction: 0000 000        src2: 00000
    instruction.rtype.rs2 = instruction_bits & ((1U << 5) - 1);
    instruction_bits >>= 5;

    // funct7: 0000 000
    instruction.rtype.funct7 = instruction_bits & ((1U << 7) - 1);
    break;
    // I-Type
    case 0x13:
      instruction.itype.rd = instruction_bits & ((1U << 5) -1); // rd : 5 bits
      instruction_bits >>= 5; // shift to the next field
      instruction.itype.funct3 = instruction_bits & ((1U<<3) - 1); //func3 : 3 bits
      instruction_bits >>= 3;
      instruction.itype.rs1 = instruction_bits & ((1U<<5) - 1); // rs1: 5 bits
      instruction_bits >>= 5;
      //print_binary(instruction.itype.imm,32);
      instruction.itype.imm = instruction_bits & ((1U << 12) - 1);
      //print_binary(instruction.itype.imm,32);
      break;
      // ecall --------------------- Itype
    case 0x73: // Transfer control to OS
      // do nothing
      
      break;
      // load instructions: ---------------- same as i-type
      case 0x03:
      instruction.itype.rd = instruction_bits & ((1U << 5) -1); // rd : 5 bits
      instruction_bits >>= 5; // shift to the next field
      instruction.itype.funct3 = instruction_bits & ((1U<<3) - 1); //func3 : 3 bits
      instruction_bits >>= 3;
      instruction.itype.rs1 = instruction_bits & ((1U<<5) - 1); // rs1: 5 bits
      instruction_bits >>= 5;
      //print_binary(instruction.itype.imm,32);
      instruction.itype.imm = instruction_bits & ((1U << 12) - 1);
      //print_binary(instruction.itype.imm,32);
      break;
      //
    // S-type
    case 0x23: {
      //instruction_bits <<= 7; // temporarily shift 7 to left
      instruction.stype.imm5   = (instruction_bits)  & 0x1F;  // imm[4:0]
      // uint32_t newimm5 = sign_extend_number(instruction.stype.imm5,5);
      // instruction.stype.imm5 = newimm5;
      // newimm5 =newimm5 << 3;
      // newimm5 &= 0x000000FF;
      // newimm5 >>= 3;
      // newimm5 =newimm5 & 0x00000000;
      // instruction.stype.imm5 = newimm5;
      // printf("***************************************************${%0x}\n", instruction_bits);
      // printf("***************************************************${%02x}\n", (instruction_bits >> 7)  & 0x01F);
      //  printf("***************************************************${%02x}\n", instruction.stype.imm5);
       

      instruction.stype.funct3 = (instruction_bits >> 5) & 0x07;
      instruction.stype.rs1    = (instruction_bits >> 8) & 0x1F;
      instruction.stype.rs2    = (instruction_bits >> 13) & 0x1F;
      instruction.stype.imm7   = (instruction_bits >> 18) & 0x7F;  // imm[11:5]
      break;
    }
    //SB type
    case 0x63: { // not 100% trusted double-check later on -- seems we do have to re-arrange imm
    
    instruction_bits <<= 7;
    instruction.sbtype.funct3 = (instruction_bits >> 12) & 0x07;
    instruction.sbtype.rs1    = (instruction_bits >> 15) & 0x1F;
    instruction.sbtype.rs2    = (instruction_bits >> 20) & 0x1F;

    // imm
    uint32_t imm12  = (instruction_bits >> 31) & 0x1;
    uint32_t imm11  = (instruction_bits >> 7)  & 0x1;
    uint32_t imm10_5= (instruction_bits >> 25) & 0x3F;
    uint32_t imm4_1 = (instruction_bits >> 8)  & 0x0F;

    // uint32_t imm5 = 
    uint32_t imm13 =
        (imm12   << 12) |
        (imm11   << 11) |
        (imm10_5 << 5)  |
        (imm4_1  << 1);
      //printf("{{{{{{{{{{{{{{{{{{{{{}}}}}}}}}}}}}}}}}}}}}:");
      //print_binary(imm13, 13);
    instruction.sbtype.imm5 = (imm13 >> 1) & 0x1F;        // imm13[5:1] --> imm5[4:0]
    instruction.sbtype.imm7 = (imm13 >> 6) & 0x7F;        // imm13[12:6] --> imm7[6:0]
    //print_binary(instruction.sbtype.imm5, 5);
    //printf("-");
    //print_binary(instruction.sbtype.imm5, 8);
    //uint32_t fullimm = (instruction.sbtype.imm7 << 5 ) | instruction.sbtype.imm5; + so much debugging T_T
    //print_binary(fullimm,32);
    //int complete_imm = (sign_extend_number(fullimm,12)) << 1;                  + cool fix :D
    //printf("\n *** complete: ");
    //print_binary(complete_imm,32);
    //printf("\n::%d}",(int)complete_imm);

    //get_branch_offset(instruction); //just to test if get_branch_offset is working

    break;
  }  
  // U-type : lui load upper immediate imm + rd
  case 0x37: { // LUI
    instruction_bits <<= 7;
    instruction.utype.rd  = (instruction_bits >> 7) & 0x1F;
    instruction.utype.imm = (instruction_bits >> 12) & 0xFFFFF; // imm[31:12]
    break;
  }
  // UJ type
  case 0x6F: { // needs testing, to understand whether imm was intended to be re-arranged or not
    instruction_bits <<= 7;
    instruction.ujtype.rd = (instruction_bits >> 7) & 0x1F;

    
    uint32_t imm20   = (instruction_bits >> 31) & 0x1; // last bit imm[20]
    uint32_t imm19_12= (instruction_bits >> 12) & 0xFF; // imm[19:12]
    uint32_t imm11   = (instruction_bits >> 20) & 0x1; // imm[11]
    uint32_t imm10_1 = (instruction_bits >> 21) & 0x3FF; //im[10:1]

    uint32_t imm21 =
        (imm20    << 20) |
        (imm19_12 << 12) |
        (imm11    << 11) |
        (imm10_1  << 1);

    
    instruction.ujtype.imm = (imm21 >> 1) & 0xFFFFF;
    break;
  }
  // case for I-type 
  default:
    exit(EXIT_FAILURE);
  }
  return instruction;
}

/* Return the number of bytes (from the current PC) to the branch label using
 * the given branch instruction */
int get_branch_offset(Instruction instruction) { // branch operations B type
  if(instruction.opcode != 0x63)
    return 0;
  // merge imm5 and imm7
  int imm = ((instruction.sbtype.imm7 << 5) | (instruction.sbtype.imm5)) << 1;
  //print_binary(imm, 32);
  //printf("{{%02x}}\n", imm);
  imm = sign_extend_number(imm,13);
  //printf("\n=============>");
  //print_binary(imm,32);
  return (int) (imm);

}

/* Returns the number of bytes (from the current PC) to the jump label using the
 * given jump instruction */
int get_jump_offset(Instruction instruction) { // 20 bit imm
  uint32_t imm = (instruction.ujtype.imm);
  imm = sign_extend_number((imm << 1),21);
  return (int)imm;

}

int get_store_offset(Instruction instruction) { // s-type imm4:0 and immimm11:5
  uint32_t imm = ((instruction.stype.imm7) << 5) | (instruction.stype.imm5);
  imm = sign_extend_number(imm,12);
  return (int) imm;

  /* YOUR CODE HERE */
  
}

void handle_invalid_instruction(Instruction instruction) {
  printf("Invalid Instruction: 0x%08x\n", instruction.bits);
}

void handle_invalid_read(Address address) {
  printf("Bad Read. Address: 0x%08x\n", address);
  exit(-1);
}

void handle_invalid_write(Address address) {
  printf("Bad Write. Address: 0x%08x\n", address);
  exit(-1);
}
void print_binary(unsigned int number, int size) // Size is in bits
{
//   if (!number)
//   {
//     putc('0', stdout);
//     return;
//   }
  if (size == 1)
  {
    putc((number & 1) ? '1' : '0', stdout);
    return;
  }
  print_binary(number >> 1, size - 1);
  putc((number & 1) ? '1' : '0', stdout);
}