idf.py build
scp build\OpenEsper.* zach@192.168.2.254:~/esp/binary/
ssh -t zach@192.168.2.254 "tmux att -t esp \; send-keys './flash.sh' Enter"