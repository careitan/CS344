#!/bin/sh
myInputFile="test_file"
while read myLine
do
  echo "$myLine"
done <$myInputFile