#!/usr/bin/env bash
set -e

for proj in $( find . -name platformio.ini | sort )
do
    echo $(dirname $proj)
    pio run -d $(dirname $proj)
done

