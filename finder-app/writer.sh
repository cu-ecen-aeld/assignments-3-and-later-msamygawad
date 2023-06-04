#!/bin/bash

if [ $# -ne 2 ]; then
	echo "Usage: $0 <writefile> <writestr>"
	exit 1
fi

writefile=$1
writestr=$2

#if [ -f "$writefile" ]
#then
#	echo "Error : $writefile is already exist in the system"
#	exit 1
#fi

if [ ! -d $(dirname "$writefile") ] 
then
	mkdir -p $(dirname "$writefile")
fi

touch $writefile
echo "$writestr" > $writefile
