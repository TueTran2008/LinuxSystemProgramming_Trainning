#!/bin/bash 
check_interger(){
    # The regular expression matches digits only 
    local f="$1"
    if ! [[ "$f" =~ ^[0-9]+$ || "$f" =~ ^[-][0-9]+$  ]]
    then
        echo "$0 - "$f" is NOT an integer. Please enter integers only."
        exit 1
    fi
}
#1. Read the input array size
read number
#2. Make sure input is provided else die with an error
if [[ "$number" == "" ]]
then
    echo "$0 - Input is missing." 
    exit 1
fi
check_interger $number 
#3. Read the input array value
read -ra array

#4. Find duplicate number
index_loop_1=0
index_loop_2=0
found_number_dup=0

while [ "$index_loop_1" -lt "$number" ]
do
   check_interger ${array[$index_loop_1]} #Filter the number in array
   index_loop_2=0

   while [ "$index_loop_2" -lt "$number" ]
   do
    if [[ ${array[$index_loop_1]} -eq ${array[$index_loop_2]} ]]
    then
        if [[ $index_loop_1 -ne $index_loop_2 ]]
        then 
            found_number_dup=1
            break
        fi
    fi
    index_loop_2=`expr $index_loop_2 + 1`
   done

   if [[ $found_number_dup -eq 1 ]]
   then
        found_number_dup=0
   else
        echo -n "${array[$index_loop_1]} "
   fi
   index_loop_1=`expr $index_loop_1 + 1`
done