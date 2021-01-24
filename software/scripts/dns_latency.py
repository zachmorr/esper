
from dns import resolver
from time import perf_counter, sleep
import random

servers = ['192.168.1.11']
queries = 100
printStats = True

with open('urls.txt') as file: 
    lines = file.readlines()
    r = resolver.Resolver()
    
    for server in servers:
        print("\nServer: %s" % server)
        r.nameservers = [server]

        _50 = _100 = _500 = _1000 = other = 0.0
        fastest = slowest = total = 0.0
        fastest_str = slowest_str = ''
        for num in range(0, queries):
            url = random.choice(lines).rstrip()
            start = perf_counter()
            try:
                r.query(url)
                stop = perf_counter()
                length = stop-start
                total += length

                if(length < fastest or fastest == 0):
                    fastest = length
                    fastest_str = url
                if(length > slowest):
                    slowest = length
                    slowest_str = url
                if(length < 0.05):
                    _50 += 1
                elif(length < 0.1):
                    _100 += 1
                elif(length < 0.3):
                    _500 += 1
                elif(length < 1):
                    _1000 += 1
                elif(length > 1):
                    other += 1

                print(num, "%s %.2fms" % (url, length*1000))
            except Exception as e:
                print("%d Exception for %s" % (num, url))
                print(e)
            sleep(0.1)
        
        if printStats:
            print("Average Time: %.2fms" % (1000*total/queries))
            print("Fastest: %s %.2fms" % (fastest_str, (fastest)*1000))
            print("Slowest: %s %.2fms" % (slowest_str, (slowest)*1000))
            print("< 50ms: %d%%" % (100*_50/queries))
            print("50 - 100ms: %d%%" % (100*_100/queries))
            print("100 - 500ms: %d%%" % (100*_500/queries))
            print("500 - 1000ms: %d%%" % (100*_1000/queries))
            print(" > 1s: %d%%" % (100*other/queries))
        
