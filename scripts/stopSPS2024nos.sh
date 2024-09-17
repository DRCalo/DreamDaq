#!/bin/bash

cd /home/dreamtest/SPS.2024.08

## Exec=myReadOutNoStop2024 -> changed to 'myReadOutNoStopV2718' on 11.09.2024
Exec=myReadOutNoStopV2718

daqPid=`ps -aef | awk '{print $2 " " $8}' | grep $Exec | awk '{print $1}'`
sleep 1

if [[ $daqPid != "" ]]; then
  echo "'$Exec' is running: PID is $daqPid"
  sleep 1
  echo "stopping process $daqPid raising semaphore"
  sleep 1
  ## incDaqSem
  stopGracely
  echo "semaphore raised"
  sleep 1

  DATE=`date +%Y.%m.%d`
  HOUR=`date +%H:%M`

  echo "'$Exec' requested to stop at " $DATE " " $HOUR
  echo "data file is" `ls -rt sps2024data.* | tail -1`
  sleep 1

else
  DATE=`date +%Y.%m.%d`
  HOUR=`date +%H:%M`

  echo "'$Exec' is not running at " $DATE " " $HOUR
  echo "last data file is" `ls -rt sps2024data.* | tail -1`
  exit 0
fi

echo "*********************************************"
echo "going to check in 2 seconds ..."
sleep 2

# check a second time
daqPid=`ps -aef | awk '{print $2 " " $8}' | grep $Exec | awk '{print $1}'`

echo "*********************************************"
if [[ $daqPid != "" ]]; then
  echo "'$Exec' is still running: PID is $daqPid"
  sleep 1
  echo "going to interrupt process $daqPid"
  sleep 1
  kill -INT $daqPid
  echo "process $daqPid asked to stop"
  sleep 1

  echo "*********************************************"
  echo "now killing process $daqPid"
  sleep 1
  kill -KILL $daqPid
  echo "sent signal SIGKILL to process $daqPid"
  sleep 1

  DATE=`date +%Y.%m.%d`
  HOUR=`date +%H:%M`

  echo "'$Exec' killed at " $DATE " " $HOUR
  echo "data file is" `ls -rt sps2024data.* | tail -1`

else
  echo "'$Exec' is no more running"
  echo "all looks fine"

fi

