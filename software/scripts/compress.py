import os
from shutil import copyfile

print('Compressing html...')

output_dir = r'firmware/components/http/src'
input_dir = r'website/public'

files = [
    (input_dir+r'/application/blacklist/index.html',        output_dir+r'/application/blacklist.html'),
    (input_dir+r'/application/home/index.html',             output_dir+r'/application/home.html'),
    (input_dir+r'/application/settings/index.html',         output_dir+r'/application/settings.html'),
    (input_dir+r'/configuration/provfinished/index.html',   output_dir+r'/configuration/provfinished.html'),
    (input_dir+r'/configuration/provsettings/index.html',   output_dir+r'/configuration/provsettings.html'),
    (input_dir+r'/configuration/provwifi/index.html',       output_dir+r'/configuration/provwifi.html'),
]

print('Creating output directory...')
if not os.path.exists(output_dir):
    os.mkdir(output_dir)
    os.mkdir(output_dir+r'/application')
    os.mkdir(output_dir+r'/configuration')

for file in files:
    with open(file[0], 'r') as src:
        with open(file[1], 'w') as dest:
            print("Writing to %s" % file[1])
            lines = src.readlines()
            for line in lines:
                dest.write(line.strip(' \t\r'))