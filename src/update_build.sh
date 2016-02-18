#!/bin/bash

BUILDFILE="build.h"
CURRBUILD=$( cat $BUILDFILE | grep "#define BUILDNO " | sed -e 's/\([^0-9*]\)//g' )
NEWBUILD=$( expr $CURRBUILD + 1 )

echo "#ifndef BUILDNO" > .tmp
echo "#define BUILDNO "$NEWBUILD >> .tmp
echo "#endif" >> .tmp

mv .tmp $BUILDFILE

echo "$NEWBUILD - remember to update the CHANGELOG! :-)"

exit 0
