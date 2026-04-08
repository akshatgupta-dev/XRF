#!/usr/bin/env bash
set -e

if [ ! -d "output" ]; then
  echo "Error: 'output' directory not found."
  exit 1
fi

output_file="output/file_totals.csv"
echo "filename,total_counts" > "$output_file"

echo "Running quality control check on CSV files..."

# Use find to recursively search all subdirectories, ignoring our metadata files
for f in $(find output -type f -name '*.csv' ! -name 'manifest.csv' ! -name 'file_totals.csv'); do
  total=$(awk -F, '$1 !~ /^#/ {s+=$6} END {print s+0}' "$f")
  echo "$f,$total" >> "$output_file"
done

echo "QC complete. Results saved to $output_file"