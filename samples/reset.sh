#!/bin/sh

cat samples/reset-cards | xargs -n2 statusupdater/statusupdater
