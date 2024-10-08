#!/bin/bash

cd /home/dreamtest/SPS.2024.08

DATE=`date +%Y.%m.%d`
HOUR=`date +%H:%M`

## Exec=myReadOutNoStop2024 -> changed to 'myReadOutNoStopV2718' on 11.09.2024
Exec=myReadOutNoStopV2718
StopScript=stopSPS2024nos.sh

daqPid=`ps -aef | awk '{print $2 " " $8}' | grep $Exec | awk '{print $1}'`
sleep 1

if [[ $daqPid != "" ]]; then
  echo "'$Exec' is already running: PID is $daqPid"
  sleep 1
  echo "please stop it before starting a new run"
  sleep 1
  echo "you can use the script '$StopScript'"
  sleep 1
  echo "data file is" `ls -rt sps2024data.* | tail -1`
  sleep 1

else
  echo "'$Exec' starting at " $DATE " " $HOUR

  $Exec &>> logfile.all.txt &
  sleep 1

  echo "'$Exec' started"
  sleep 1

  echo "data file is" `ls -rt sps2024data.* | tail -1`
  sleep 1

  echo "please use the script '$StopScript' to stop the program"

  daqPid=`ps -aef | awk '{print $2 " " $8}' | grep $Exec | awk '{print $1}'`
  sleep 1

  if [[ $daqPid != "" ]]; then
    echo "'$Exec' is running: PID is $daqPid"
    sleep 1
    echo "data file is" `ls -rt sps2024data.* | tail -1`
    sleep 1
    echo -n " ... 3"
    sleep 1
    echo -n " ... 2"
    sleep 1
    echo -n " ... 1"
    sleep 1
    echo

    top -p $daqPid
  else
    echo "'$Exec' is not running"
    sleep 1
    echo "Please take care: something went wrong"
    sleep 1
    echo "last data file is" `ls -rt sps2024data.* | tail -1`
    sleep 1
  fi

fi
