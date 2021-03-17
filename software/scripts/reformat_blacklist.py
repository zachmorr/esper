import os

outputFile = r'firmware/components/url/default_blacklist.txt'
inputFile = r'DefaultBlacklist/blacklist.txt'

with open(inputFile, 'rb') as input:
    with open(outputFile, 'wb') as output:
        for url in input.readlines():
            url = url.strip(b'\r\n')
            url_length = len(url)
            length_byte = bytes([url_length])
            output.write(length_byte+url)

print("Reformated Blacklist file.")
