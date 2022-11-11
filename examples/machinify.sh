#!/bin/env bash

set -e

files=$(ls ./*.hwasm)

for file in $files
do
    echo "(\"assembling\") $file -> ./$(basename $file .hwasm).asic"
    perl -ne '@a=split;for(@a){print chr(hex($_))}' $file > $(basename $file .hwasm).asisc
    xxd $(basename $file .hwasm).asisc
done
