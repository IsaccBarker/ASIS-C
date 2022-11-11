#!/bin/env bash

set -e

files=$(ls ./*.hwasm)

for file in $files
do
    echo "(\"assembling\") $file -> ./$(basename $file .hwasm).asic"
    awk '{if ($1 != "#") print $0}' $file > $file.processed
    perl -ne '@a=split;for(@a){print chr(hex($_))}' $file.processed > $(basename $file .hwasm).asisc
    rm $file.processed
    xxd $(basename $file .hwasm).asisc
done
