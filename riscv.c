#include "riscv.h"
#include <assert.h>
#include <getopt.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

/* WARNING: DO NOT CHANGE THIS FILE.
 YOU PROBABLY DON'T EVEN NEED TO LOOK AT IT... */

// Pointer to simulator memory
Byte *memory;
#define MAX_SIZE 50

void execute(Processor *processor, int prompt, int print) {
  /* fetch an instruction */
  uint32_t instruction_bits = load(memory, processor->PC, LENGTH_WORD);

  /* interactive-mode prompt */
  if (prompt) {
    if (prompt == 1) {
      printf("simulator paused,enter to continue...");
      while (getchar() != '\n')
        ;
    }

    printf("%08x: ", processor->PC);
    decode_instruction(instruction_bits);
  }

  execute_instruction(instruction_bits, processor, memory);

  // enforce $0 being hard-wired to 0
  processor->R[0] = 0;

  // print trace
  if (print) {
    int i, j;

    for (i = 0; i < 8; i++) {
      for (j = 0; j < 4; j++) {
        printf("r%2d=%08x ", i * 4 + j, processor->R[i * 4 + j]);
      }

      puts("");
    }

    printf("\n");
  }
}

void init_args(Processor *processor, char *arg) {
  char *token = strtok(arg, ",");
  int i = 0;
  while (token != NULL) {
    processor->R[i + 10] = (int32_t)strtol(token, NULL, 16);
    token = strtok(NULL, ",");
    i++;
  }
}

int load_file(uint8_t *mem, size_t memsize, int startaddr, const char *filename,
              int disasm) {
  FILE *file = fopen(filename, "r");
  char line[MAX_SIZE];
  int instruction, offset = 0;
  int programsize = 0;
  while (fgets(line, MAX_SIZE, file) != NULL) {
    instruction = (int32_t)strtol(line, NULL, 16);
    programsize++;
    mem[startaddr + offset] = instruction & 0xFF;
    mem[startaddr + offset + 1] = (instruction >> 8) & 0xFF;
    mem[startaddr + offset + 2] = (instruction >> 16) & 0xFF;
    mem[startaddr + offset + 3] = (instruction >> 24) & 0xFF;

    if (disasm) {
      printf("%08x: ", startaddr + offset);
      decode_instruction((uint32_t)instruction);
    }

    offset += 4;
  }
  return programsize;
}

int main(int argc, char **argv) {
  /* options */
  int opt_disasm = 0, opt_regdump = 0, opt_interactive = 0, opt_exit = 0,
      opt_init_reg = 0;

  /* the architectural state of the CPU */
  Processor processor;
  /* initialize the CPU */
  /* zero out all registers */
  // int i;
  for (int i = 0; i < 32; i++) {
    processor.R[i] = 0;
  }

  char *data_file = NULL;
  // int a1;
  /* parse the command-line args */
  int c;
  while ((c = getopt(argc, argv, "dvrites:a:")) != -1) {
    switch (c) {
    case 'd':
      opt_disasm = 1;
      break;
    case 'v':
      opt_init_reg = 1;
      break;
    case 'r':
      opt_regdump = 1;
      break;
    case 'i':
      opt_interactive = 1;
      break;
    case 't':
      opt_interactive = 2;
      break;
    case 'e':
      opt_exit = 1;
      break;
    case 's':
      data_file = optarg;
      break;
    case 'a':
      init_args(&processor, optarg);
      // opt_a1 = 1;
      // Read hex value as integer
      // a1 = (int32_t)strtol(optarg, NULL, 16);
      break;
    default:
      fprintf(stderr, "Bad option %c\n", c);
      return -1;
    }
  }

  if (opt_init_reg) {
    for (int i = 0; i < 32; i++) {
      processor.R[i] = 4;
    }
  }

  /* Set the global pointer to 0x3000. We arbitrarily call this the middle of
   * the static data segment */
  processor.R[3] = 0x3000;

  /* Set the stack pointer near the top of the memory array */
  processor.R[2] = 0xEFFFF;

  /* make sure we got an executable filename on the command line */
  if (argc <= optind) {
    fprintf(stderr, "Give me an executable file to run!\n");
    return -1;
  }

  /* load the executable into memory */
  assert(memory == NULL);
  memory = calloc(MEMORY_SPACE, sizeof(uint8_t)); // allocate zeroed memory
  assert(memory != NULL);
  int prog_numins = 0;
  /* SEt the PC to 0x1000 */
  processor.PC = 0x1000;
  prog_numins =
      load_file(memory, MEMORY_SPACE, processor.PC, argv[optind], opt_disasm);
  // Loading data
  if (data_file != NULL) {
    load_file(memory, MEMORY_SPACE, processor.R[3], data_file, 0);
  }
  // for (int i = processor.R[3]; i < processor.R[3] + data_size * 4; i += 4) {
  //   Word result = load(memory, i, LENGTH_WORD);
  //   printf("%08x, %08x \n", i, result);
  // }

  /* if we're just disassembling,exit here */
  if (opt_disasm) {
    return 0;
  }



  // if (opt_a1) {
  //   processor.R[11] = a1;
  // }

  int simins = 0;

  if (opt_exit) {
    /* simulate forever! */
    while (1) {
      execute(&processor, opt_interactive, opt_regdump);
    }
  } else {
    /* Either simulate for program instructions */
    while (simins < prog_numins) {
      execute(&processor, opt_interactive, opt_regdump);
      simins++;
    }
  }
  return 0;
}
