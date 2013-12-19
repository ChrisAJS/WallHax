#!/bin/sh

./codescanner/codescanner.sh $1 | csvjsonparser/process | xargs -n2 statusupdater/statusupdater
