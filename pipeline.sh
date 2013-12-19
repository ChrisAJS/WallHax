#!/bin/sh

./codescanner/codescanner.sh samples/codescanner-sample.png | csvjsonparser/process | xargs -n2 statusupdater/statusupdater
