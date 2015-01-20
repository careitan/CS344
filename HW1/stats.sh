# CS 344 Homework 1
# (Craig) Allan Reitan

#Validate Arguments
$CMD=$1;
$FILE=$2;

$ERR=0;

#set an error flag if any fundamental requirements are not met.
if [ ($CMD -ne "-r" || $CMD -ne "-c") ]; then
  $ERR=1;
elif [ $FILE -eq "" ]; then
  $ERR=1;
elif ["$3" -ne ""]; then
  $ERR=1;
fi

#take action based on input conditions.
if [ $ERR -eq 1 ]; then
  echo "Usage: stats {-rows|-cols} [file]";
else
  echo "Hello World";
fi

exit