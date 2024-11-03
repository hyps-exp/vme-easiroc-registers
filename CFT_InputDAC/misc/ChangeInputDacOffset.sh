#!/bin/sh

# instruction of options
#        -a : DAC values of all the channes are changed corresponding to the offset value.
#        -u : DAC values of the upper channes (0--31) are changed corresponding to the offset value.
#        -d : DAC values of the lower channes (32--63) are changed corresponding to the offset value.
#        -i<ch_i> -f<ch_f>: DAC values of the range <ch_i> to <ch_f> are changed corresponding to the offset value.

FLAG_AUD=0
FLAG_IF=0
while getopts ":audi:f:" OPT
do
  case $OPT in
      a) FLAG_A=1; FLAG_AUD=`expr $FLAG_AUD + 1`
	  ;;
      u) FLAG_U=1; FLAG_AUD=`expr $FLAG_AUD + 1`
	  ;;
      d) FLAG_D=1; FLAG_AUD=`expr $FLAG_AUD + 1`
	  ;;
      i) FLAG_I=1; ch_i=$OPTARG; FLAG_IF=`expr $FLAG_IF + 1`
	  ;;
      f) FLAG_F=1; ch_f=$OPTARG; FLAG_IF=`expr $FLAG_IF + 1`
	  ;;
     \?) 
     echo "USAGE: $0 [option: audif] [DAC_file] [offset_value]" 1>&2
     echo "Failed. (>A<)"
     exit 1
	  ;;
  esac
done

shift $((OPTIND - 1))

dir=/home/DAQ/vme-easiroc/Controller/CFT_InputDAC
log_file=$dir/misc/log/ChangeInputDacOffset.log
echo "Moving to the directory of /home/DAC/vme-easiroc/Controller/CFT_InputDAC..."
cd $dir
echo "###########################################################" >> $log_file
date >> $log_file
echo "Running the program \"ChangeInputDacOffset\"..." | tee -a $log_file
if [ $FLAG_AUD -eq 1 ] && [ $FLAG_IF -eq 0 ]; then
    if [ $FLAG_A ]; then
	echo "> ./misc/ChangeInputDacOffset -a -- $1 $2" | tee -a $log_file
	./misc/ChangeInputDacOffset -a -- $1 $2 2>&1 | tee -a $log_file
    elif [ $FLAG_U ]; then
	echo "> ./misc/ChangeInputDacOffset -u -- $1 $2" | tee -a $log_file
	./misc/ChangeInputDacOffset -u -- $1 $2 2>&1 | tee -a $log_file
    elif [ $FLAG_D ]; then
	echo "> ./misc/ChangeInputDacOffset -d -- $1 $2" | tee -a $log_file
	./misc/ChangeInputDacOffset -d -- $1 $2 2>&1 | tee -a $log_file
    else
	echo "USAGE: $0 [option: audif] [DAC_file] [offset_value]" 1>&2 | tee -a $log_file
	echo "Failed. (>A<)" | tee -a $log_file
	exit 1
    fi
elif [ $FLAG_IF -eq 2 ] && [ $FLAG_AUD -eq 0 ]; then
    echo "> ./misc/ChangeInputDacOffset -i$ch_i -f$ch_f -- $1 $2" | tee -a $log_file
    ./misc/ChangeInputDacOffset -i$ch_i -f$ch_f -- $1 $2 2>&1 | tee -a $log_file
else
    echo "USAGE: $0 [option: audif] [DAC_file] [offset_value]" 1>&2 | tee -a $log_file
    echo "Failed. (>A<)" | tee -a $log_file
    exit 1
fi

if [ $? -eq 0 ]; then
    prefix=`date +%Y%m%d%H%M%S`
    echo "Changing the file names..." | tee -a $log_file
    chmod 664 $1
    mv $1 ./backup/$1.org.$prefix
    mv $1.offset_added $1
    chmod 444 $1
    echo "$1 has moved to $dir/backup/$1.org.$prefix." | tee -a $log_file
    echo "$1.offset_added has changed to be $1." | tee -a $log_file
    echo "Done!" | tee -a $log_file
    echo "#### file diff check ####" | tee -a $log_file
    diff ./backup/$1.org.$prefix $1 | tee -a $log_file
    echo "#########################" | tee -a $log_file
else 
    echo "The program of \"ChangeInputDacOffset\" returned error." | tee -a $log_file
    echo "Failed. (>A<)" | tee -a $log_file
fi
echo "###########################################################" >> $log_file
