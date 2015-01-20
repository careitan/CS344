#!/bin/sh
c=$1
f=$2
e=0
if [[ $c -ne "-r" || $c -ne "-c" ]]; then
  $e=1
elif [[ -z "$f" ]]; then
  $e=1
elif [[ -z "$3" ]]; then
  $e=1
else
  $e=1
fi
if [[ $e -eq 1 ]]; then
  echo "Usage: stats {-rows|-cols} [file]"
else
  echo "Hello World"
fi
exit