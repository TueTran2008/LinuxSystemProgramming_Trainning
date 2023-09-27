#!/bin/bash
echo "Enter some numbers:"
read -a myarray 
echo "There were ${#myarray[@]} numbers and index 4 was ${myarray[4]}"