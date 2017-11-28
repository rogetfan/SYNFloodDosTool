#!/bin/bash

# find pid

pid_id=`ps -ef | grep -v grep | grep './tool -t' | awk '{print $2}'`

kill $pid_id