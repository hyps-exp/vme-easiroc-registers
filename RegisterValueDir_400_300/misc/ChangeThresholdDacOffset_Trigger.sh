#!/bin/sh

dir=/home/DAQ/vme-easiroc/Controller/RegisterValueDir_400_300
#dir=/home/DAQ/vme-easiroc/Controller/RegisterValueDir_400_300_test
log_file=$dir/misc/log/ChangeThresholdDacOffset.log
echo "Moving to the directory of" $dir"..."
cd $dir
echo "###########################################################" >> $log_file
date >> $log_file
echo "Running the program \"ChangeThresholdDacOffset\"..." | tee -a $log_file
command="./misc/ChangeThresholdDacOffset"

for i in 17 19 21 22 23 24 26 27 28 30 31 32 33 34 35 36 38 39 41 43 44 46 48 49 ; do
    filename=RegisterValue_$i.yml
    if [ $i -eq 16 ]; then
	continue;
    elif [ $i -eq 22 ]; then
	echo "> $command -d -- $filename $1" | tee -a $log_file
	$command -d -- $filename $1 2>&1 | tee -a $log_file
    elif [ $i -eq 23 ]; then
	echo "> $command -d -- $filename $1" | tee -a $log_file
	$command -d -- $filename $1 2>&1 | tee -a $log_file
    elif [ $i -eq 26 ]; then
	echo "> $command -u -- $filename $1" | tee -a $log_file
	$command -u -- $filename $1 2>&1 | tee -a $log_file
    elif [ $i -eq 27 ]; then
	echo "> $command -u -- $filename $1" | tee -a $log_file
	$command -u -- $filename $1 2>&1 | tee -a $log_file
    elif [ $i -eq 32 ]; then
	echo "> $command -d -- $filename $1" | tee -a $log_file
	$command -d -- $filename $1 2>&1 | tee -a $log_file
    elif [ $i -eq 34 ]; then
	echo "> $command -u -- $filename $1" | tee -a $log_file
	$command -u -- $filename $1 2>&1 | tee -a $log_file
    elif [ $i -eq 35 ]; then
	echo "> $command -u -- $filename $1" | tee -a $log_file
	$command -u -- $filename $1 2>&1 | tee -a $log_file
    else
	echo "> $command -- $filename $1" | tee -a $log_file
	$command -- $filename $1 2>&1 | tee -a $log_file
    fi
done
echo "Done!" | tee -a $log_file
echo "###########################################################" >> $log_file
