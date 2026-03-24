#!/bin/bash

cd "$(dirname "$0")"

echo "--- Работаем в директории: $(pwd) ---"

make clean
make

REPORT="result.txt"
echo "--- Отчет о тестировании (Дата: $(date)) ---" > "$REPORT"

chmod +x create_A.sh
./create_A.sh
echo "1. Создан файл A" >> "$REPORT"

./myprogram fileA fileB
echo "2. Файл A скопирован в B (sparse)" >> "$REPORT"

gzip -f -k fileA
gzip -f -k fileB
echo "3. Файлы сжаты gzip" >> "$REPORT"

gzip -cd fileB.gz | ./myprogram fileC
echo "4. B.gz распакован в C через программу" >> "$REPORT"

./myprogram -b 100 fileA fileD
echo "5. A скопирован в D (блок 100)" >> "$REPORT"

echo -e "\n--- Результаты stat ---" >> "$REPORT"
for f in fileA fileA.gz fileB fileB.gz fileC fileD; do
    if [ -f "$f" ]; then
        echo "Файл: $f" >> "$REPORT"
        stat "$f" >> "$REPORT"
        echo "----------------" >> "$REPORT"
    fi
done
