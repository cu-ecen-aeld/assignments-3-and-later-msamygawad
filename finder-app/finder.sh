#!/bin/sh


if [ $# -ne 2 ]; then
	echo "Usage: $0 <File_Directory> <Search_String>"
	exit 1
fi

FILES_DIR=$1
SEARCHING_STR=$2

if [ ! -d "$FILES_DIR" ]
then
	echo "Error : $FILES_DIR is not a directory"
	exit 1
fi

NO_OF_FILES=$(find "$FILES_DIR" -type f | wc -l)
MATCHES_COUNT=$(grep -r "$SEARCHING_STR" "$FILES_DIR" | wc -l)

echo "The number of files are $NO_OF_FILES and the number of matching lines are $MATCHES_COUNT" 
 
