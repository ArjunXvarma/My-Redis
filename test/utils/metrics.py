# utils/metrics.py
import csv

def export_to_csv(filename, data, headers):
    with open(filename, 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(headers)
        writer.writerows(data)
