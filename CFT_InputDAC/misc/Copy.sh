#!/bin/sh

dir=/home/DAQ/vme-easiroc/Controller/CFT_InputDAC
target=$dir/$1
cd $dir

if [ -e $target ]; then
    cp $1 InputDAC_tmp.yml
    echo "Done!"
else 
    echo "File $target was not found."
    echo "Failed. (>A<)"
fi
