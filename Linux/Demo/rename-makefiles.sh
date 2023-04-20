#!/usr/bin/bash

rename_makefile() {
    echo Renaming makefile $1
    b=$(basename $1)
    d=$(dirname $1)
    git mv $1 $d/Makefile
}

MAKE_FILES=$(find . -name makefile)

for f in $MAKE_FILES ; do
    rename_makefile $f
done

