#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

int main(int argc, char *argv[]) {
    int opt;
    size_t block_size = 4096;

    // разбор параметров
    while ((opt = getopt(argc, argv, "b:")) != -1) {
        if (opt == 'b') block_size = atoi(optarg);
    }

    int fd_in = STDIN_FILENO;
    int fd_out;

    // типы аргументов: 1 аргумент - stdout, 2 - файлы
    if (argc - optind == 1) {
        fd_out = open(argv[optind], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    } else if (argc - optind == 2) {
        fd_in = open(argv[optind], O_RDONLY);
        fd_out = open(argv[optind + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    } else {
        fprintf(stderr, "Usage: %s [-b block_size] [input] output\n", argv[0]);
        exit(1);
    }

    if (fd_in < 0 || fd_out < 0) {
        perror("open");
        exit(1);
    }

    char *buf = malloc(block_size);
    ssize_t n_read;
    off_t current_size = 0;

    while ((n_read = read(fd_in, buf, block_size)) > 0) {
        int is_zero = 1;
        for (int i = 0; i < n_read; i++) {
            if (buf[i] != 0) {
                is_zero = 0;
                break;
            }
        }

        if (is_zero) {
            // прыгаем вперед, вместо обычной записи
            if (lseek(fd_out, n_read, SEEK_CUR) == -1) {
                perror("lseek");
                exit(1);
            }
        } else {
            if (write(fd_out, buf, n_read) != n_read) {
                perror("write");
                exit(1);
            }
        }
        current_size += n_read;
    }

    // устанавливаем итоговый логический размер
    if (ftruncate(fd_out, current_size) == -1) {
        perror("ftruncate");
    }

    free(buf);
    close(fd_out);
    if (fd_in != STDIN_FILENO) close(fd_in);

    return 0;
}