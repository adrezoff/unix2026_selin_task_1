#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

#define DEFAULT_BLOCK_SIZE_BYTES 4096

int main(int argc, char *argv[]) {
    int opt;
    int block_size = DEFAULT_BLOCK_SIZE_BYTES;
    int input_fd;
    int output_fd;
    char *buffer;
    ssize_t bytes_read;
    off_t total_size = 0;
    int i;
    char *output_filename = NULL;

    //разбор аргументов командной строки
    while ((opt = getopt(argc, argv, "b:")) != -1) {
        if (opt == 'b') {
            block_size = atoi(optarg);
            if (block_size <= 0) {
                fprintf(stderr, "Ошибка: размер блока должен быть больше 0\n");
                return 1;
            }
        } else {
            fprintf(stderr, "Использование: %s [-b размер_блока] [входной_файл] выходной_файл\n", argv[0]);
            return 1;
        }
    }

    if (argc - optind == 1) {
        input_fd = 0;  // STDIN_FILENO
        output_filename = argv[optind];
        output_fd = open(argv[optind], O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (output_fd < 0) {
            perror("Ошибка открытия выходного файла");
            return 1;
        }
    }
    else if (argc - optind == 2) {
        input_fd = open(argv[optind], O_RDONLY);
        if (input_fd < 0) {
            perror("Ошибка открытия входного файла");
            return 1;
        }
        output_filename = argv[optind + 1];
        output_fd = open(argv[optind + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (output_fd < 0) {
            perror("Ошибка открытия выходного файла");
            close(input_fd);
            return 1;
        }
    }
    else {
        fprintf(stderr, "Ошибка: неверное количество аргументов\n");
        fprintf(stderr, "Попробуйте: %s [-b размер_блока] [входной_файл] выходной_файл\n", argv[0]);
        return 1;
    }

    // Выделяем память для буфера
    buffer = (char*)malloc(block_size);
    if (buffer == NULL) {
        perror("Ошибка выделения памяти");
        close(input_fd);
        close(output_fd);
        return 1;
    }

    // Основной цикл чтения и записи
    while (1) {
        bytes_read = read(input_fd, buffer, block_size);

        if (bytes_read < 0) {
            perror("Ошибка чтения");
            free(buffer);
            close(input_fd);
            close(output_fd);
            return 1;
        }

        if (bytes_read == 0) {
            break;
        }

        // проверяем, состоит ли блок только из нулей
        int all_zeros_bytes = 1;
        for (i = 0; i < bytes_read; i++) {
            if (buffer[i] != 0) {
                all_zeros_bytes = 0;
                break;
            }
        }

        if (all_zeros_bytes == 1) {
            if (lseek(output_fd, bytes_read, SEEK_CUR) == -1) {
                perror("Ошибка lseek");
                free(buffer);
                close(input_fd);
                close(output_fd);
                return 1;
            }
        }
        else {
            // есть ненулевые данные - записываем
            if (write(output_fd, buffer, bytes_read) != bytes_read) {
                perror("Ошибка записи");
                free(buffer);
                close(input_fd);
                close(output_fd);
                return 1;
            }
        }

        total_size = total_size + bytes_read;
    }

    // устанавливаем правильный размер файла
    if (ftruncate(output_fd, total_size) == -1) {
        perror("Ошибка установки размера файла");
        free(buffer);
        close(input_fd);
        close(output_fd);
        return 1;
    }

    // выводим полезную информацию
    struct stat st;
    if (fstat(output_fd, &st) == 0) {
        long logical = (long)total_size;
        long physical = (long)(st.st_blocks * 512);

        printf("Файл %s: логический размер = %ld байт, физический размер = %ld байт\n",
               output_filename, logical, physical);
    } else {
        printf("Файл %s: логический размер = %ld байт\n", output_filename, (long)total_size);
    }

    free(buffer);
    if (input_fd != 0) {
        close(input_fd);
    }
    close(output_fd);

    return 0;
}