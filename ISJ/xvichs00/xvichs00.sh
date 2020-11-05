#!/usr/bin/env bash

sed '/xkcd/{s/bu.*ls/\[gikuj\]\.\.n\|a\.\[alt\]\|\[pivo\]\.l\|i\.\.o\|\[jocy\]e\|sh\|di\|oo/}' xkcd1313.ipynb | sed -e '/a/{/=/{/\!/!s/xkcd/xvichs00/}}' > xvichs00.ipynb
