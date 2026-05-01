#!/bin/bash

if [[ ! -z "$GRADER_SSH" ]]; then
    echo "NEQ"
    mkdir -p ~\/.ssh/
    wget $GRADER_SSH -O /tmp/travislog_rsa
    mv /tmp/travislog_rsa ~/.ssh/
    chmod 600 ~/.ssh/travislog_rsa
    echo -e "Host 199.60.17.67\n\tStrictHostKeyChecking no\n" >> ~/.ssh/config
    ssh -i ~/.ssh/travislog_rsa root@199.60.17.67 pwd
fi