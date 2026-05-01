#!/bin/bash


if [ ! -z "$GRADER_SSH" ] &&  [ ! -z "$ASS_ROOT" ]; then
  echo $TRAVIS_REPO_SLUG
  FILENAME=`basename $TRAVIS_REPO_SLUG`
    scp -i ~/.ssh/travislog_rsa $TRAVIS_BUILD_DIR/${FILENAME}.log root@199.60.17.67:~/CMPT295/$ASS_ROOT/PASS/
  scp -i ~/.ssh/travislog_rsa $TRAVIS_BUILD_DIR/${FILENAME}_Grade.json root@199.60.17.67:~/CMPT295/$ASS_ROOT/PASS/
fi
