#!/bin/bash

for DIR in * ; do 
    if [ ! -d $DIR ] ; then 
        continue
    fi
    cd $DIR
    make clean 
    cd ..

done


