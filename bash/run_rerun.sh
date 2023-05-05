#!/bin/bash

# Runs something again, if it died
#
# Usage:
# 
# run_rerun.sh "/some/random/long.sh command line"
#  -- checks ps -aux if it's running
#  -- if not, sleep 10s, and run it
# 
# (c) 2023 John Mueller, MIT license
#

# get current path
SCRIPTPATH="$( cd "$(dirname "$0")" ; pwd -P )"

FINDFILE=$(basename -- "$1")

if pgrep -f "$FINDFILE" | grep -v "^$$\$" &> /dev/null 2>&1; then
    echo "( $FINDFILE ) is already running, exiting."
    exit 0
fi

# it is not running
echo "it ( $FINDFILE ) is NOT running"
echo "Waiting 10s"
sleep 10s

USE_VENV=0
cd $SCRIPTPATH
cd $(dirname "$1")
if [[ -f ".venv/pyvenv.cfg" ]]; then
    echo "Activating venv ..."
    source .venv/bin/activate
    USE_VENV=1
fi

echo "Starting $1 ..."
MAIN_FILE=$(basename $1)
if [ "${MAIN_FILE: -3}" == ".py" ]; then
    python3 $MAIN_FILE
else
    $MAIN_FILE
fi

if [ $USE_VENV == "1" ]; then
    echo "Deactivating venv"
    deactivate
fi
