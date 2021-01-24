import os
from shutil import copyfile

print('Compressing html...')
files = [
    r'application/homepage.html',
    r'application/blacklist.html',
    r'application/settings.html',
    r'configuration/selectwifi.html',
    r'configuration/connected.html',
    r'configuration/finished.html',
]


output_dir = r'components/http/src'
source_dir = r'website'

print('Creating output directory...')
if not os.path.exists(output_dir):
    os.mkdir(output_dir)
    os.mkdir(output_dir+r'/application')
    os.mkdir(output_dir+r'/configuration')

for file in files:
    filename, extension = os.path.splitext(file)
    input_path = os.path.join(source_dir, file)
    output_path = os.path.join(output_dir, file)

    with open(input_path, 'r') as src:
        with open(output_path, 'w') as dest:
            print("Writing to %s" % output_path)
            if extension == '.html':
                lines = src.readlines()
                for line in lines:
                        dest.write(line.strip(' \t\r'))
            else:
                copyfile(input_path, output_path)