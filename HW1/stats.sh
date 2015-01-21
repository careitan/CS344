#!/bin/sh
set -x
c=$( echo $1 | cut -c -2 )
f=$2
e=0
if [[ $c -ne "-r" ]]; then
  e=1
elif [[ $c -ne "-c" ]]; then
  e=1
elif [[ -z "$f" ]]; then
  e=1
elif [[ ! -r "$f" ]]; then
  echo "$0: Cannot read $f">2
  exit 1
elif [[ -n "$3" ]]; then
  e=1
else
  e=0
fi
if [[ $e -eq 1 ]]; then
  echo "Usage: stats {-rows|-cols} [file]"
  exit 1
fi
linecount=1;number=0
avg=0;avgList=0
median=0;medianList=0
if [[ $c == "-r" ]]; then
  echo "Average Median"
fi
while read myLine
do
  if [[ $c == "-r" ]]; then
    median=$( echo $myLine|sed -e 's/ /\n/g'|sort -n )
    number=$( echo $myLine|sed -e 's/ /\n/g'|sort -n|wc -l )
    avg=$( expr `echo $myLine | sed -e 's/ / + /g' -e "s,$, \) / $number," -e "s/^/\( /"` )
    if [[ $( expr $number % 2 ) -ne 0 ]]; then
      median=$( echo $median|cut -d ' ' -f $( expr \( $number / 2 \) + 1 ) )
    else
      median=$( echo $median|cut -d ' ' -f $( expr \( $number / 2 \) ) )
    fi
    echo -e "$avg \t $median"
  else
    #http://forum.linuxcareer.com/threads/1645-Calculate-column-average-using-bash-shell
    columns=$( echo $myLine|sed -e 's/ /\n/g'|sort -n|wc -l )
    for (( i = 1; i <= columns; i++ )); do
      avg=0
      median=0
      for j in $( echo awk '{ print $i; }' $f ); do
        avg=$(echo $avg+$j|bc )
        median=$( echo $median " " $j )
        number=$[$number + 1]
      done
      avg=$(echo "scale=0;$avg/$number" | bc)
      median=$( echo $median|sed -e 's/ /\n/g'|sort -n )
      if [[ $( expr $number % 2 ) -ne 0 ]]; then
        median=$( echo $median|cut -d ' ' -f $( expr \( $number / 2 \) + 1 ) )
      else
        median=$( echo $median|cut -d ' ' -f $( expr \( $number / 2 \) ) )
      fi
      avgList=$( echo -e $avgList "   " $avg )
      medianList=$( echo -e $medianList "   " $median )
    done
  fi
done<$f
if [[ $c == "-c" ]]; then
  echo -e "Averages:\n\n$avgList\n\nMedians:\n\n$medianList\n"
  exit 0
else
  exit 0
fi
exit 0