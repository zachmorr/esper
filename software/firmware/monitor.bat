ssh -t zach@192.168.2.254 "tmux att -t esp \; send-keys 'idf_monitor.py --port /dev/ttyUSB0 OpenEsper.elf' Enter"