#!/bin/bash
python scripts/compress.py
python scripts/reformat_blacklist.py
idf.py build -C firmware/
