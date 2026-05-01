#!/bin/bash

non_zero=0

function run_cmd_with_check() {
  "$@"
  if [[ $? -ne 0 ]] 
  then
    printf "failed"
    ((non_zero++))
  fi
}

run_cmd_with_check make riscv
run_cmd_with_check python3 driver.py -D ./code/out
cat LOG.md >> LOG

exit ${non_zero}