#!/bin/bash 
#fucntion sum two input number
sum()
{
	local a=$1
	local b=$2
	local sum=$(( a + b))
	echo $sum
}
#fucntion minus two input number
minus()
{
	local a=$1
	local b=$2
	local sum=$(( a - b))
	echo $sum
}
#fucntion minus two input number
multiple()
{
	local a=$1
	local b=$2
	local sum=$(( a * b))
	echo $sum
}
#fucntion divide two input number
# divide by 0 is the special case -> the program itself handle 
divide()
{
	local a=$1
	local b=$2
	local sum=$(( a / b))
	echo $sum
}
#function check if input is an interger
check_interger(){
    # The regular expression matches digits only 
    local f="$1"
    if ! [[ "$f" =~ ^[0-9]+$ || "$f" =~ ^[-][0-9]+$  ]]
    then
        echo "$0 - "$f" is NOT an integer. Please enter integers only."
        exit 1
    fi
}
#Check input arguments. If smaller than 3 -> return the function
if [[ $# -lt 3 ]]
then 
    echo "Not enough argument"
    exit 1
fi
# Make sure input is provided else die with an error
if [[ "$1" == "" ]]
then
    echo "$0 - Input is missing." 
    exit 1
fi

check_interger $1
check_interger $3

case $2 in 
    "+")
    sum $1 $3
    ;;
    "-")
    minus $1 $3
    ;;
    "x")
    multiple $1 $3
    ;;
    "/")
    divide $1 $3
    ;;
    *)
    echo "Invalid operation"
    ;;
esac