#!/bin/bash

cd /home/dreamtest/SPS.2024.08.DEBUG

Exec=$1

daqPid=`ps -aef | awk '{print $2 " " $8}' | grep $Exec | awk '{print $1}'`
sleep 1

if [[ $daqPid != "" ]]; then
  echo "'$Exec' is running: PID is $daqPid"
  sleep 1
  echo "stopping process $daqPid raising semaphore"
  sleep 1
  incDaqSem
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
  sleep 1
fi

echo "waiting for 2 seconds then going to check ..."
sleep 2

exit 0

# check a second time
daqPid=`ps -aef | awk '{print $2 " " $8}' | grep $Exec | awk '{print $1}'`

if [[ $daqPid != "" ]]; then
  echo "'$Exec' is still running: PID is $daqPid"
  sleep 1
  echo "killing process $daqPid"
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

