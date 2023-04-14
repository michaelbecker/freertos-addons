#!/usr/bin/bash

remove_co_routine() {
    echo Removing CO-ROUTINES from $1
    sed -i '/CO_ROUTINE/d' $1
    sed -i '/Co-routine/d' $1
}

CFG_FILES=$(find . -name FreeRTOSConfig.h)

for f in $CFG_FILES ; do
    remove_co_routine $f
done

