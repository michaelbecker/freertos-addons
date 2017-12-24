#!/bin/bash

for DIR in * ; do 
    if [ ! -d $DIR ] ; then 
        continue
    fi
    cd $DIR
    if ! make -j4 ; then 
       echo "Make Failed in $DIR"
       exit 1
    fi 
  
    cd ..

done


