#!/bin/sh

./codescanner/codescanner.sh capture.jpg | ticketpositionparser/process #| xargs -n2 statusupdater/statusupdater
