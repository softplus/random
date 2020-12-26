#!/bin/bash

# Schedule a command some minutes, assuming it isn't already scheduled.
#
# Uses 'at'
# 
# Usage:
# 
# schedule_in.sh 5 "/some/random/long.sh command line"
#  -- schedules that command to run at earliest in 5 minutes
#  -- if already scheduled, it will cancel that one, and reschedule
# 
# Useful if you want to schedule something to run a certain time after a last
# trigger event. Eg, "send me last photos from camera 5 minutes after action recognized"
# Also useful to rate-limit commands.
# 

INMINUTES="$1"
SCHEDULE="$2"
TMPFILE=/tmp/$jobs.tmp

# get jobs list
echo "Checking jobs for '$SCHEDULE' ..."
at -l | awk '{print $1}' >"$TMPFILE"

cat "$TMPFILE" | while read line
do
  ISONE=$(at -c $line | grep "$SCHEDULE")
  if [ "$ISONE" != "" ]; then
    echo "at job $line matches, canceling it."
    atrm $line
  fi 
done

rm "$TMPFILE"

# schedule
echo "Scheduling new '$SCHEDULE' in $INMINUTES minutes"
echo "$SCHEDULE" | at now + "$INMINUTES" minutes

echo "Current schedule of jobs:"
at -l

echo "Done."


