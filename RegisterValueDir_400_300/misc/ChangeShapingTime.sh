#!/bin/sh

# instruction of options
#        -a : DAC values of all the channes are changed corresponding to the offset value.
#        -u : DAC values of the upper channes (0--31) are changed corresponding to the offset value.
#        -d : DAC values of the lower channes (32--63) are changed corresponding to the offset value.
#        -i<board_i> -f<board_f>: VME-EASIROC board of the range <ch_i> to <ch_f> are changed corresponding to the offset value.

FLAG_AUDHL=0
FLAG_IF=0
opt_command=
while getopts ":audhli:f:" OPT
do
  case $OPT in
      a) FLAG_A=1; opt_command+="";   FLAG_AUDHL=`expr $FLAG_AUDHL + 1`
	  ;;
      u) FLAG_U=1; opt_command+="u"; FLAG_AUDHL=`expr $FLAG_AUDHL + 1`
	  ;;
      d) FLAG_D=1; opt_command+="d"; FLAG_AUDHL=`expr $FLAG_AUDHL + 1`
	  ;;
      h) FLAG_H=1; opt_command+="h"; FLAG_AUDHL=`expr $FLAG_AUDHL + 1`
	  ;;
      l) FLAG_L=1; opt_command+="l"; FLAG_AUDHL=`expr $FLAG_AUDHL + 1`
	  ;;
      i) FLAG_I=1; board_i=$OPTARG;  FLAG_IF=`expr $FLAG_IF + 1`
	  ;;
      f) FLAG_F=1; board_f=$OPTARG;  FLAG_IF=`expr $FLAG_IF + 1`
	  ;;
     \?) 
     echo "USAGE: ChangeShapingTime [option] [offset_value]" 1>&2
     echo "Failed. (>A<)" 1>&2
     exit 1
	  ;;
  esac
done

shift $((OPTIND - 1))

dir=/home/DAQ/vme-easiroc/Controller/RegisterValueDir_400_300
#dir=/home/DAQ/vme-easiroc/Controller/RegisterValueDir_400_300_test
log_file=$dir/misc/log/ChangeShapingTime.log
echo "Moving to the directory of" $dir"..."
cd $dir
echo "###########################################################" >> $log_file
date >> $log_file
echo "Running the program \"ChangeShapingTime\"..." | tee -a $log_file
if [ $FLAG_AUDHL -gt 0 ] && [ $FLAG_IF -eq 2 ]; then
    if [ $FLAG_A -eq 1 ]; then
	command="./misc/ChangeShapingTime --"
    else
	command="./misc/ChangeShapingTime -$opt_command --"
    fi
    for i in $(seq $board_i $board_f); do
	if [ $i -eq 18 ] || [ $i -eq 95 ]; then continue; fi
	filename=RegisterValue_$i.yml
	echo "> $command $filename $1" | tee -a $log_file
	$command $filename $1 2>&1 | tee -a $log_file
    done
else
    echo "USAGE: ChangeShapingTime [option] [offset_value]" | tee -a $log_file
    echo "Failed. (>A<)" | tee -a $log_file
    exit 1
fi

if [ $? -eq 0 ]; then
    echo "Done!" | tee -a $log_file
else 
    echo "The program of \"ChangeThresholdDacOffset\" returned error." | tee -a $log_file
    echo "Failed. (>A<)" | tee -a $log_file
fi
echo "###########################################################" >> $log_file
