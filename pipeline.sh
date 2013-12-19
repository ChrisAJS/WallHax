#!/bin/sh

convert capture.jpg capture.png
rm capture.jpg
./codescanner/codescanner.sh capture.png | ticketpositionparser/process | xargs -n2 statusupdater/statusupdater


