#!/bin/bash
DIRNAME=`dirname $0`
echo Processing $@
LD_LIBRARY_PATH=/usr/local/lib ./$DIRNAME/codescanner $@
