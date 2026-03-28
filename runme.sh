#!/bin/bash

make > /dev/null

echo "=== BEGIN STEPS ===" > result.txt

bash create_test_file_A.sh

echo "Copy A -> B (sparse)" >> result.txt
./myprogram A B

echo "gzip A and B" >> result.txt
gzip -c A > A.gz
gzip -c B > B.gz

echo "Restore B.gz -> C through program" >> result.txt
gzip -cd B.gz | ./myprogram C

echo "Copy A -> D with block 100" >> result.txt
./myprogram -b 100 A D

echo "" >> result.txt
echo "=== STAT RESULTS ===" >> result.txt

stat A >> result.txt
stat A.gz >> result.txt
stat B >> result.txt
stat B.gz >> result.txt
stat C >> result.txt
stat D >> result.txt