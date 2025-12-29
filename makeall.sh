#!/usr/bin/env bash

for proj in $( find . -name platformio.ini | sort )
do
    echo $(dirname $proj)
    pio run -s -d $(dirname $proj)
done

