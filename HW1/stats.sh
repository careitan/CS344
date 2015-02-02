#!/bin/sh
#set -x
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
median='';medianList=0
if [[ $c == "-r" ]]; then
  echo "Average Median"
fi
if [[ $c == "-r" ]]; then
  while read myLine
  do
    median=$( echo $myLine|sed -e 's/ /\n/g'|sort -n )
    number=$( echo $myLine|sed -e 's/ /\n/g'|sort -n|wc -l )
    avg=$( expr `echo $myLine | sed -e 's/ / + /g' -e "s,$, \) / $number," -e "s/^/\( /"` )
    if [[ $( expr $number % 2 ) -ne 0 ]]; then
      median=$( echo $median|cut -d ' ' -f $( expr \( $number / 2 \) + 1 ) )
    else
      median=$( echo $median|cut -d ' ' -f $( expr \( $number / 2 \) ) )
    fi
    echo -e "$avg \t $median"
  done<$f
else
        # Find max number of columns in file
        columns=$(cat test_file | awk 'BEGIN {$mynf=0} {if ($mynf < NF) $mynf=NF} END {print $mynf}')

        # For each column
        for (( i=1; i <= columns; i++ )); do
                # Get the column
                myCol[$i]=$(cat test_file | awk "{print $i}")

                # Find median
                median=$(echo ${myCol[$i]} | sed -e 's/ /\n/g' | sort -n) || 0
                number=$(echo ${myCol[$i]} | sed -e 's/ /\n/g' | sort -n | wc -l) || 0
                if [[ $( expr $number % 2 ) -ne 0 ]]; then
                        median=$(echo $median | cut -d ' ' -f $(expr \( $number / 2 \) + 1 )) || 0
                else
                        median=$(echo $median | cut -d ' ' -f $(expr \( $number / 2 \))) || 0
                fi

                # Find Average
                avg=$(expr `echo ${myCol[$i]} | sed -e 's/ / + /g' -e "s,$, \) / $number," -e "s/^/\( /"`) || 0

                # Display results
                echo -e "$avg \t $median"
        done
#       echo -e "Averages:\n\n$avgList\n\nMedians:\n\n$medianList\n"
#       exit 0
fi
exit 0