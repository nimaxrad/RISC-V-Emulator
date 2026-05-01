SOURCES := utils.c part1.c part2.c riscv.c
HEADERS := types.h utils.h riscv.h
PWD := $(shell pwd)
CUNIT := -L $(PWD)/CUnit-install/lib -I $(PWD)/CUnit-install/include -llibcunit
CFLAGS := -g  -Wall


ASM_TESTS := simple multiply random

all: riscv part1 part2
	@echo "=============All tests finished============="

.PHONY: part1 %_disasm

riscv: $(SOURCES) $(HEADERS) out
	gcc $(CFLAGS) -o $@ $(SOURCES)

out:
	@mkdir -p ./code/out

# Part 1 Tests

#part1: riscv $(addsuffix _disasm, $(ASM_TESTS))
	@echo "---------Disassembly Tests Complete---------"

#%_disasm: code/input/%.input code/ref/%.solution riscv
#	@./riscv -d $< > code/out/test.dump
#	@diff $(word 2, $^) code/out/test.dump && echo "$@ TEST PASSED!" || echo "$@ TEST FAILED!"

part2: riscv $(addsuffix _execute, $(ASM_TESTS))
	@echo "-----------Execute Tests Complete-----------"

# %_execute: code/input/%.input code/ref/%.solution riscv
# 	@./riscv -r -e $< > code/out/$*.trace
# 	@python2.7 part2_tester.py $*

test-utils:
	gcc $(CFLAGS) -DTESTING -o test-utils test_utils.c utils.c $(CUNIT)
	./test-utils
	rm -f test-utils

clean:
	rm -f riscv
	rm -f *.o
	rm -f test-utils
	rm -rf code/out
