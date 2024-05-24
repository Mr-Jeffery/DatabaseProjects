import csv
import chardet

with open('bus_lines.txt', 'rb') as f:
    result = chardet.detect(f.read())  # or readline if the file is large

with open('bus_lines.txt', 'r', encoding=result['encoding']) as in_file:
    stripped = (line.strip() for line in in_file)
    lines = (line.replace('"', '').split("\t") for line in stripped if line)
    with open('bus_lines.csv', 'w') as out_file:
        writer = csv.writer(out_file)
        writer.writerows(lines)