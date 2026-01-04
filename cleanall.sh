#!/usr/bin/env bash

for pio in $(find . -name .pio -type d )
do
    rm -r $pio
done

