#! /usr/bin/bash

python scripts/compress.py
python scripts/reformat_blacklist.py
idf.py build
scp build/EsperDNS.bin zach@upgrade.esperdns.co:~/esper