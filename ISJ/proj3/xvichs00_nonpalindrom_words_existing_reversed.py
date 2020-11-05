#!/usr/bin/env python3

import fileinput

words = {}
result = []
for line in fileinput.input():
    line = line.rstrip()
    inverted_line = line[::-1]
    if( line != inverted_line ):
        words[line] = 1
        if inverted_line in words:
            result.append(line)
            result.append(inverted_line)

print(sorted(result))
