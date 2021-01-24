import os

outputFile = r'components/url/default_blacklist.txt'
inputFile = r'scripts/blacklist.txt'

with open(inputFile, 'rb') as input:
    with open(outputFile, 'wb') as output:
        for url in input.readlines():
            url = url.strip(b'\r\n')
            url_length = len(url)
            length_byte = bytes([url_length])
            #print(length_byte + url)
            output.write(length_byte+url)

print("Reformated Blacklist file.")
