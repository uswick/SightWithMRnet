#!/bin/sh
echo "copying..."
DEST=/home/usw/Install/sight/temp_repo/SightWithMRnet
cp -r  /media/sf_Appcode/SightNew/SightNew/sight/mrnet $DEST
cp /media/sf_Appcode/SightNew/SightNew/sight/sight_structure.C $DEST
cp /media/sf_Appcode/SightNew/SightNew/sight/process.C $DEST
cp /media/sf_Appcode/SightNew/SightNew/sight/process.h $DEST
cp /media/sf_Appcode/SightNew/SightNew/sight/sight_structure_internal.h $DEST
cp /media/sf_Appcode/SightNew/SightNew/sight/Makefile $DEST
cp /media/sf_Appcode/SightNew/SightNew/sight/examples/Makefile $DEST/examples


