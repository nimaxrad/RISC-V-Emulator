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


TRAVIS_BUILD_DIR=$PWD
cd ${TRAVIS_BUILD_DIR}
rm FAILED
rm SUCCESS

git log --pretty=format:'Name:%cn %nHash:%H%nTimestamp=%ci %n' -n 1 >> LOG
run_cmd_with_check bash run.sh
FILENAME=`basename $PWD`

if [ "$non_zero" -eq "0"  ] 
then
  echo "" >> SUCCESS
  rm -f ${TRAVIS_BUILD_DIR}/$FILENAME.log.failed
  mv LOG ${TRAVIS_BUILD_DIR}/$FILENAME.log.success
else
  echo "" >> FAILED
  rm -f ${TRAVIS_BUILD_DIR}/$FILENAME.log.success
  mv LOG ${TRAVIS_BUILD_DIR}/$FILENAME.log.failed
fi
