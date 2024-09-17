#!/bin/bash

cd /home/dreamtest/SPS.2024.08

## Exec=myReadOutNoStop2024 -> changed to 'myReadOutNoStopV2718' on 11.09.2024
Exec=myReadOutNoStopV2718

daqPid=`ps -aef | awk '{print $2 " " $8}' | grep $Exec | awk '{print $1}'`

if [[ $daqPid != "" ]]; then
  echo "'$Exec' is running: PID is $daqPid"
  sleep 1
  echo "killing process $daqPid"
  sleep 1
  kill -9 $daqPid
  echo "process $daqPid killed"
  sleep 1

  DATE=`date +%Y.%m.%d`
  HOUR=`date +%H:%M`

  echo "'$Exec' killed at " $DATE " " $HOUR
  echo "data file is" `ls -rt sps2024data.* | tail -1`

else
  DATE=`date +%Y.%m.%d`
  HOUR=`date +%H:%M`

  echo "'$Exec' is not running at " $DATE " " $HOUR
  echo "last data file is" `ls -rt sps2024data.* | tail -1`
fi

