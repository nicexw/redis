#!/bin/sh
while [ 0 ]
do
free -m
free -m |grep Mem |awk '{print $3-$7}'
sleep 1
done
