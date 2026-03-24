#!/bin/bash
FILE="fileA"
LAST_1_POS=410241024

# перенаправляем поток в файл
> $FILE

# пишем единицу в начало, в index=10000 и в конец
printf "1" | dd of=$FILE bs=1 seek=0 conv=notrunc status=none
printf "1" | dd of=$FILE bs=1 seek=10000 conv=notrunc status=none
printf "1" | dd of=$FILE bs=1 seek=LAST_1_POS conv=notrunc status=none