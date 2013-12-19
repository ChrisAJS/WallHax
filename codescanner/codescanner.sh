#!/bin/bash
DIRNAME=`dirname $0`

if [ 'x`uname`'='xDarwin' ]; then
	DYLD_LIBRARY_PATH=/usr/local/lib
else
	LD_LIBRARY_PATH=/usr/local/lib
fi

echo Processing $@
LD_LIBRARY_PATH=/usr/local/lib ./$DIRNAME/codescanner $@
