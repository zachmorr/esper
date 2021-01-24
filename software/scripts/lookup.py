import sys
from dns import resolver

ip = '192.168.1.11'

if len(sys.argv) >= 3:
    print("Querying {} {} times for {}".format(ip, sys.argv[2], sys.argv[1]))
    r = resolver.Resolver()
    r.nameservers = [ip]
    r.lifetime = 1
    for n in range(int(sys.argv[2])):
        res = r.query(sys.argv[1])
        print("Received " + res[0].to_text())