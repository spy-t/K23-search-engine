#! /usr/bin/env python

import sys

if __name__ == '__main__':
    files = {}
    filename = sys.argv[1]
    with open(filename, "r") as f:
        for word in f.readlines():
            word_len = len(word) - 1
            if files.get(word_len) is None:
                files[word_len] = open(f'{word_len}-{filename}', "a")
            nf = files.get(word_len)
            nf.write(word)

    for _, f in files.items():
        f.close()

