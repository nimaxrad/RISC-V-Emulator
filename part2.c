#include <stdio.h> // for stderr
#include <stdlib.h> // for exit()
#include "types.h"
#include "utils.h"
#include "riscv.h"

void execute_rtype(Instruction, Processor *);
void execute_itype_except_load(Instruction, Processor *);
void execute_branch(Instruction, Processor *);
void execute_jal(Instruction, Processor *);
void execute_load(Instruction, Processor *, Byte *);
void execute_store(Instruction, Processor *, Byte *);
void execute_ecall(Processor *, Byte *);
void execute_lui(Instruction, Processor *);
void print_binary(unsigned int number, int size); // Size is in bits
void execute_lswc(Instruction, Processor *, Byte *);

void execute_instruction(uint32_t instruction_bits, Processor *processor,Byte *memory) {    
    Instruction instruction = parse_instruction(instruction_bits);
    switch(instruction.opcode) {
        case 0x2a:
        //lswc
            execute_lswc(instruction, processor, memory);
            break;
        case 0x33:
            execute_rtype(instruction, processor);
            break;
        case 0x13:
            execute_itype_except_load(instruction, processor);
            break;
        case 0x73:
            execute_ecall(processor, memory);
            break;
        case 0x63:
            execute_branch(instruction, processor);
            break;
        case 0x6F:
            execute_jal(instruction, processor);
            break;
        case 0x23:
            execute_store(instruction, processor, memory);
            break;
        case 0x03:
            execute_load(instruction, processor, memory);
            break;
        case 0x37:
            execute_lui(instruction, processor);
            break;
        default: // undefined opcode
            handle_invalid_instruction(instruction);
            exit(-1);
            break;
    }
}

void execute_rtype(Instruction instruction, Processor *processor) {
    int64_t rd;
    switch (instruction.rtype.funct3){
        case 0x0:
            switch (instruction.rtype.funct7) {
                case 0x0:
                  // Add
                  processor->R[instruction.rtype.rd] =
                      ((sWord)processor->R[instruction.rtype.rs1]) +
                      ((sWord)processor->R[instruction.rtype.rs2]);
                    processor->PC += 4;
                  break;
                case 0x1:
                  // Mul
                  processor->R[instruction.rtype.rd] =
                      ((sWord)processor->R[instruction.rtype.rs1]) *
                      ((sWord)processor->R[instruction.rtype.rs2]);
                      processor->PC += 4;
                  break;
                case 0x20:
                //rd = rs1 - rs2
                // Sub
                    processor->R[instruction.rtype.rd] = ((sWord)processor->R[instruction.rtype.rs1]) - 
                        ((sWord)processor->R[instruction.rtype.rs2]);
                    processor->PC += 4;
                    break;
                default:
                    handle_invalid_instruction(instruction);
                    exit(-1);
                    break;
            }
            break;
        case 0x1:
            switch (instruction.rtype.funct7) {
                case 0x0:
                    // SLL
                    // rd = rs1 << rs2
                    //print_binary(instruction.bits, 32);
                    processor->R[instruction.rtype.rd] = (processor->R[instruction.rtype.rs1]) << ((uint32_t)processor->R[instruction.rtype.rs2]);
                    processor->PC += 4;
                    break;
                case 0x1:
                    // MULH
                    // rd [63:32]
                    //sDouble hello;
                    
                    rd = ((sDouble)processor->R[instruction.rtype.rs1]) * ((sDouble)processor->R[instruction.rtype.rs2]);
                    processor->R[instruction.rtype.rd] = (uint32_t)((int64_t)rd >> 32);
                    processor->PC += 4;
                    break;
            }
            break;
        case 0x2:
            // SLT
            //rd = (rs1 < rs2)?1:0
            if((sWord)(processor->R[instruction.rtype.rs1]) < (sWord)(processor->R[instruction.rtype.rs2])){
                processor->R[instruction.rtype.rd] = 0x1;
            }else{
                processor->R[instruction.rtype.rd] = 0x00000000;
            }
            processor->PC += 4;
            break;
        case 0x4:
            switch (instruction.rtype.funct7) {
                case 0x0:
                    // XOR
                    processor->R[instruction.rtype.rd] = processor->R[instruction.rtype.rs1] ^ processor->R[instruction.rtype.rs2];
                    processor->PC += 4;
                    break;
                case 0x1:
                    // DIV
                    // rtype
                    // div DIV R 0110011 0x4 0x01 rd = rs1 / rs2
                    processor->R[instruction.rtype.rd] = ((sWord)processor->R[instruction.rtype.rs1]) / ((sWord)processor->R[instruction.rtype.rs2]);
                    processor->PC += 4;
                    break;
                default:
                    handle_invalid_instruction(instruction);
                    exit(-1);
                    break;
            }
            break;
        case 0x5:
            switch (instruction.rtype.funct7) {
                case 0x0:
                // SRL unsigned
                // srl Shift Right Logical R 0110011 0x5 0x00 rd = rs1 >> rs2
                // srl rd, rs1 >> rs2      
                processor->R[instruction.rtype.rd] = (Word)processor->R[instruction.rtype.rs1] >> (Word)processor->R[instruction.rtype.rs2];
                processor->PC += 4;
                    break;
                case 0x20:
                    // SRA arithmetic shift right
                    processor->R[instruction.rtype.rd] = (sWord)processor->R[instruction.rtype.rs1] >> (Word)processor->R[instruction.rtype.rs2];
                processor->PC += 4;

                    break;
                default:
                    handle_invalid_instruction(instruction);
                    exit(-1);
                break;
            }
            break;
        case 0x6:
            switch (instruction.rtype.funct7) {
                case 0x0:
                    // OR
                    processor->R[instruction.rtype.rd] = processor->R[instruction.rtype.rs1] | processor->R[instruction.rtype.rs2];
                    processor->PC += 4;
                    break;
                case 0x1:
                    // REM
                    //rem Remainder R 0110011 0x6 0x01 rd = rs1 % rs2
                    processor->R[instruction.rtype.rd] = (sWord)processor->R[instruction.rtype.rs1] % (sWord)processor->R[instruction.rtype.rs2];
                    processor->PC += 4;
                    break;
                default:
                    handle_invalid_instruction(instruction);
                    exit(-1);
                    break;
            }
            break;
        case 0x7:
            // AND
            processor->R[instruction.rtype.rd] = processor->R[instruction.rtype.rs1] & processor->R[instruction.rtype.rs2];
            processor->PC += 4;
            break;
        default:
            handle_invalid_instruction(instruction);
            exit(-1);
            break;
    }
}

void execute_itype_except_load(Instruction instruction, Processor *processor) {
    uint32_t funct7;
    uint32_t immid;
    uint32_t immi;

    switch (instruction.itype.funct3) {
        case 0x0:
            // ADDI
            //addi ADD Immediate I 0010011 0x0 rd = rs1 + imm
            //processor->R[instruction.rtype.rd] = processor->R[instruction.rtype.rs1] & processor->R[instruction.rtype.rs2];
            // processor->PC += 4;
            processor->R[instruction.itype.rd] = (processor->R[instruction.itype.rs1]) + sign_extend_number(instruction.itype.imm,12);
            processor->PC += 4;
            break;
        case 0x1:
            // SLLI
            // shift left logical imm
            // rd = rs1 << imm[0:4]
            immi = (instruction.bits >> 20) & 0x1F; // 5 bit mask

            processor->R[instruction.itype.rd] = processor->R[instruction.itype.rs1] << (Word)(immi);
            processor->PC += 4;
            break;
        case 0x2:
            // STLI 
            //slti?? set less than immediate
            // rd = (rs1 < imm)?1:0
            
            if((sWord)(processor->R[instruction.itype.rs1])<(sign_extend_number(instruction.itype.imm,12))){
                processor->R[instruction.itype.rd] = 0x1;
            }else{
                processor->R[instruction.itype.rd] = 0x0;
            }
            processor->PC += 4;
            break;
        case 0x4:
            // XORI
            //xor immediate
            //rd = rs1 ˆ imm
            processor->R[instruction.itype.rd] = processor->R[instruction.itype.rs1] ^ sign_extend_number(instruction.itype.imm,12);
            processor->PC += 4;


            break;
        case 0x5: // --------------- two cases : srli srai
            //rd = rs1 >> imm[0:4]

            funct7 = (instruction.bits >> 25) & 0x7F;  // 0x7F = 7 bits mask
            immid = (instruction.bits >> 20) & 0x1F; // 5 bit mask
            if (funct7 == 0x00) {
                // srli
                // logical
                processor->R[instruction.itype.rd] = (Word)(processor->R[instruction.itype.rs1]) >> immid; 
            }
            else if (funct7 == 0x20) {   // 0100000 binary = 0x20
                //srai
                // arithmetic - signed
                processor->R[instruction.itype.rd] = (sWord)(processor->R[instruction.itype.rs1]) >> immid; 
            }
            processor->PC += 4;
            break;
        case 0x6:
            // ORI
                processor->R[instruction.itype.rd] = processor->R[instruction.itype.rs1] | sign_extend_number(instruction.itype.imm,12);
                processor->PC += 4;
                break;
        case 0x7:
            // ANDI
                processor->R[instruction.itype.rd] = processor->R[instruction.itype.rs1] & sign_extend_number(instruction.itype.imm,12);
                processor->PC += 4;
            break;
        default:
            handle_invalid_instruction(instruction);
            break;
    }
}

void execute_ecall(Processor *p, Byte *memory) {
    Register i;
    
    // syscall number is given by a0 (x10)
    // argument is given by a1
    switch(p->R[10]) {
        case 1: // print an integer
            printf("%d",p->R[11]);
            break;
        case 4: // print a string
            for(i=p->R[11];i<MEMORY_SPACE && load(memory,i,LENGTH_BYTE);i++) {
                printf("%c",load(memory,i,LENGTH_BYTE));
            }
            break;
        case 10: // exit
            printf("exiting the simulator\n");
            exit(0);
            break;
        case 11: // print a character
            printf("%c",p->R[11]);
            break;
        default: // undefined ecall
            printf("Illegal ecall number %d\n", p->R[10]);
            exit(-1);
            break;
    }
    p->PC += 4;
}

void execute_branch(Instruction instruction, Processor *processor) {
    // The program counter is updated explicitly by branches (SB type), JAL (J type), 
    // and JALR (I type). These instructions include immediates that need to be extracted and sign extended.
    // Make sure you pass part 1 and are able to disassemble
    // these instructions appropriately; without this its futile to attempt part 2
    int fullimm = get_branch_offset(instruction);

    switch (instruction.sbtype.funct3) {
        

        case 0x0:
            // BEQ
            // if(rs1==rs2) pc += imm;
            if(processor->R[instruction.sbtype.rs1] == processor->R[instruction.sbtype.rs2]){
                processor->PC += fullimm;
            }else{
                processor->PC += 4;
            }

            break;
        case 0x1:
            // BNE
            if(processor->R[instruction.sbtype.rs1] != processor->R[instruction.sbtype.rs2]){
                processor->PC += fullimm;
            }else{
                processor->PC += 4;
            }
            break;
        default:
            handle_invalid_instruction(instruction);
            exit(-1);
            break;
    }
}

void execute_load(Instruction instruction, Processor *processor, Byte *memory) {
        int base_index = (int)(processor->R[instruction.itype.rs1]) + sign_extend_number(instruction.itype.imm,12);
        uint8_t m;
        uint16_t v;
        // printf("{{{:}}}");
        // print_binary(instruction.itype.imm, 32);
        // printf("\n");
        // print_binary(sign_extend_number(instruction.itype.imm,12), 32);
        // printf("\n");
        
    switch (instruction.itype.funct3) {
        case 0x0:
            // LB
            // single byte load
            // processor->R[instruction.itype.rd] = load(memory, sign_extend_number(processor->R[instruction.itype.imm],12) + 
            //             processor->R[instruction.itype.rs1], LENGTH_BYTE);
            m= memory[base_index];
            processor->R[instruction.itype.rd] = sign_extend_number(m, 8);
            processor->PC += 4;
            break;
        case 0x1:
            // LH
            //half word
            // processor->R[instruction.itype.rd] = load(memory, sign_extend_number(processor->R[instruction.itype.imm],12) + 
            //             processor->R[instruction.itype.rs1], LENGTH_HALF_WORD);
            v= memory[base_index] | (memory[base_index+1] << 8);
            processor->R[instruction.itype.rd] = sign_extend_number(v, 16);
                                                        
            processor->PC += 4;
            break;
        case 0x2:
        //load word
            //load(memory,i,LENGTH_BYTE)
            //load(memoty,)
            // LW
            // print_binary(instruction.bits, 32);
            //         processor->R[instruction.rtype.rd] = (processor->R[instruction.rtype.rs1]) << ((uint32_t)processor->R[instruction.rtype.rs2]);
            //         processor->PC += 4;
    //         unsigned int opcode : 7;
	// unsigned int rd : 5;	
	// unsigned int funct3 : 3;
	// unsigned int rs1 : 5;
	// unsigned int imm : 12;

            processor->R[instruction.itype.rd] = memory[base_index] | (memory[base_index+1] << 8) 
                                                                    | (memory[base_index+2] << 16)
                                                                    | (memory[base_index+3] << 24)
            ;
            // printf(" mem[addr]:");
            // print_binary(processor->R[instruction.itype.rd],32);
            // printf("------}");
            // uint32_t exval = *((uint32_t*)(memory + base_index));
            // printf("\n real:");
            // print_binary(exval,32);
            // printf("------}");
            // processor->R[instruction.itype.rd] = load(memory, sign_extend_number(processor->R[instruction.itype.imm],12) + 
            //             processor->R[instruction.itype.rs1], LENGTH_WORD);
            processor->PC += 4;
            break;
        default:
            handle_invalid_instruction(instruction);
            break;
    }

}
void execute_lswc(Instruction instruction, Processor *processor, Byte *memory){
    // if MEM[rs1] < rs2:
    //          MEM[rs1] = rs2
    // rd = MEM[rs1]
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // char* ptr;
    // ptr =  processor->R[instruction.rtype.rs1]; // mem[rs1]


    int32_t base_index = (int) (processor->R[instruction.rtype.rs1]);
    int32_t r1 = (int)(memory[base_index] | (memory[base_index+1] << 8) 
                                                                    | (memory[base_index+2] << 16)
                                                                    | (memory[base_index+3] << 24));//int
    
    int r2 = (int)(processor->R[instruction.rtype.rs2]);
    if(r1 < r2){
        //*((uint32_t*)(memory + base_index)) = processor->R[instruction.rtype.rs2];
        memory[base_index] = (uint8_t)(processor->R[instruction.rtype.rs2]);
        memory[base_index + 1] = (uint8_t)((processor->R[instruction.rtype.rs2]) >> 8);
        memory[base_index + 2] = (uint8_t)((processor->R[instruction.rtype.rs2]) >> 16);
        memory[base_index + 3] = (uint8_t)((processor->R[instruction.rtype.rs2]) >> 24);
        
        
        
        //processor->R[instruction.rtype.rd] = r1;
        
    }
    processor->R[instruction.rtype.rd] = r1;

    
    processor->PC +=4;

    // int base_index = processor->R[instruction.itype.rs1] + sign_extend_number(instruction.itype.imm,12);
    //     uint8_t m;
    //     uint16_t v;
    // switch (instruction.itype.funct3) {
    //     case 0x0:
    //         // LB
    //         // single byte load
    //         // processor->R[instruction.itype.rd] = load(memory, sign_extend_number(processor->R[instruction.itype.imm],12) + 
    //         //             processor->R[instruction.itype.rs1], LENGTH_BYTE);
    //         m= memory[base_index];
    //         processor->R[instruction.itype.rd] = sign_extend_number(m, 8);
    //         processor->PC += 4;

}
void execute_store(Instruction instruction, Processor *processor, Byte *memory) {
    // imm[0:4] imm[5:11]
int fullimm = get_store_offset(instruction);

    switch (instruction.stype.funct3) {
        case 0x0:
            // SB
            // store byte
            store(memory, processor->R[instruction.stype.rs1] + fullimm, LENGTH_BYTE, processor->R[instruction.stype.rs2]);
            processor->PC += 4;

            break;
        case 0x1:
            // SH
            // M[rs1+imm][0:15] = rs2[0:15]
            //void store(Byte *memory, Address address, Alignment alignment, Word value)
            // sh rs2, imm(rs1)	mem[rs1+imm][0:15] = rs2

            store(memory, processor->R[instruction.stype.rs1] + fullimm, LENGTH_HALF_WORD, processor->R[instruction.stype.rs2]);
            processor->PC += 4;

            break;
        case 0x2:
            // SW
            //store word
            store(memory, processor->R[instruction.stype.rs1] + fullimm, LENGTH_WORD, processor->R[instruction.stype.rs2]);
            processor->PC += 4;

            break;
        default:
            handle_invalid_instruction(instruction);
            exit(-1);
            break;
    }
}

void execute_jal(Instruction instruction, Processor *processor) {
    /* YOUR CODE HERE */
    processor->R[instruction.ujtype.rd] = (processor->PC + 4);
    processor->PC += get_jump_offset(instruction);

}

void execute_lui(Instruction instruction, Processor *processor) {
    // lui
    // processor->R[instruction.itype.rs1], LENGTH_WORD);
    // rd = imm << 12
    // U type
    processor->R[instruction.utype.rd] = instruction.utype.imm << 12;
    processor->PC += 4;
    /* YOUR CODE HERE */
}

void store(Byte *memory, Address address, Alignment alignment, Word value) {
    // gets a address : char*
    // gets size : address : offset : word
    // get value : word
    // store at *(mem + addr) = value;

   //Word output = ;
    switch(alignment){
        case LENGTH_WORD:
            *((uint32_t*)(memory + address)) = value;
            break;
        case LENGTH_HALF_WORD:
            *((uint16_t*)(memory + address)) = (uint16_t) value;
            break;
        case LENGTH_BYTE:
            *((uint8_t*)(memory + address)) = (uint8_t) value;
            break;
        default:
            printf("error\n");
            break;
    }
    
    /* YOUR CODE HERE */
    
}
//byte: 8 bits
//address : 32 bits
//alignemnt: enum
Word load(Byte *memory, Address address, Alignment alignment) {
    // address = uint32_t
    // memory = * to memory : same as char(uint8_t)*
    // alignemnt = size to read
    //char* tmp_ptr;
    // load a 32but word from the memory using the address
    // example: load(memory,i,LENGTH_BYTE)
    // This function accepts an address and a size and returns the next -size- bytes starting at the given addres
    // LENGTH_BYTE = 1,
    // LENGTH_HALF_WORD = 2,
    // LENGTH_WORD = 4,
    Word output = 0x0;
    // printf("------------------------------------------------------------: %d\n", address);

    switch(alignment){
        case LENGTH_WORD:

            // load a 32 bit word
            // memory: a pointer to uint_8 : same as a char*
            // address: offset
            // memory is the actual pointer to the real memory of the simulator

            // for(int i=0;i<4;i++){ // load 4 bytes
            //     uint32_t tmp = *()(memory + (uint32_t)address + i);
            //     tmp &= 0x00000001;
            //     tmp <<= (i*8); // shift left by 1 byte
            //     output |= tmp;
            // }
            
            output = *((uint32_t*)((char*)memory + (int)address));
            // printf("\n++++++++++\n");
            // print_binary(output,32);

            // printf("\n++++++++++\n");  
            break;
        case LENGTH_HALF_WORD:
            output = sign_extend_number((*((uint16_t*)((char*)memory + (int)address))),16);
            break;
        case LENGTH_BYTE:
            output = sign_extend_number((*((uint8_t*)((char*)memory + (int)address))),8);
            break;
        default:
            printf("error\n");
            break;
    }
    
    /* YOUR CODE HERE */
    return output;
}
