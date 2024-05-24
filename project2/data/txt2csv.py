# import csv
# import chardet

# with open('bus_lines.txt', 'rb') as f:
#     result = chardet.detect(f.read())  # or readline if the file is large

# with open('bus_lines.txt', 'r', encoding=result['encoding']) as in_file:
#     stripped = (line.strip() for line in in_file)
#     lines = (line.replace('"', '').split("\t") for line in stripped if line)
#     with open('bus_lines.csv', 'w') as out_file:
#         writer = csv.writer(out_file)
#         writer.writerows(lines)
import csv

def remove_duplicate(filename):
    with open(filename, 'r') as csvfile:
        reader = csv.reader(csvfile)
        header = next(reader)  # Skip the header

        new_rows = []
        for row in reader:
            intro = row[4]
            # Split the intro into two halves
            mid = len(intro) // 2
            first_half = intro[:mid]
            second_half = intro[mid:]
            # If the two halves are the same, keep only the first half
            if first_half == second_half:
                row[4] = first_half
            new_rows.append(row)

    # Write the new rows back to the file
    with open(filename, 'w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(header)
        writer.writerows(new_rows)

remove_duplicate('stations.csv')