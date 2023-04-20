#!/usr/bin/bash

remove_lines() {
    echo Removing lines from $1
    sed -i '/portGET_RUN_TIME_COUNTER_VALUE/d' $1
    sed -i '/portCONFIGURE_TIMER_FOR_RUN_TIME_STATS/d' $1
}

CFG_FILES=$(find . -name FreeRTOSConfig.h)

for f in $CFG_FILES ; do
    remove_lines $f
done


