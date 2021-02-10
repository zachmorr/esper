#! /usr/bin/bash

website/hugo/hugo -s website/
python scripts/compress.py
# idf.py -C firmware -p /dev/ttyUSB0 flash monitor