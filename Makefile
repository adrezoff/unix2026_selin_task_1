# Переменные для компилятора и флагов
CC = gcc
CFLAGS = -Wall -Wextra -Werror

# Цель по умолчанию (просто запускает сборку программы)
all: myprogram

# Правило сборки исполняемого файла
myprogram: main.c
	$(CC) $(CFLAGS) main.c -o myprogram

# Правило для очистки проекта от временных файлов и результатов тестов
clean:
	rm -f myprogram fileA fileB fileC fileD fileA.gz fileB.gz result.txt