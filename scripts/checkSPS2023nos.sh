#!/bin/bash

cd /home/dreamtest/SPS.2023.06

DATE=`date +%Y.%m.%d`
HOUR=`date +%H:%M`

Exec=myReadOutNoStop2023

echo "Checking '$Exec' state at " $DATE " " $HOUR

daqPid=`ps -aef | awk '{print $2 " " $8}' | grep $Exec | awk '{print $1}'`
sleep 1

if [[ $daqPid != "" ]]; then
  echo "'$Exec' is running: PID is $daqPid"
  echo "data file is" `ls -rt sps2023data.* | tail -1`
  sleep 1
else
  echo "'$Exec' is not running"
  echo "last data file is" `ls -rt sps2023data.* | tail -1`
  sleep 1
fi

