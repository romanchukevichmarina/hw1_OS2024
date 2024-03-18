#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>

const int buffer_size = 5000;

int findSequence(char* str, int l, char* ans, int n) {
    int counter = 0;
    int last_c = str[l-1];
    for (int i = l-2; i >= 0; --i) {
        if (last_c > str[i]) {
            if(counter == 0){
                ans[n - 1 - counter++] = last_c;
            }
            ans[n - 1 - counter] = str[i];
            ++counter;
            if (counter == n){
                return 1;
            }
        } else {
            counter = 0;
        }
        last_c = str[i];
        
    }
    return -1;
}

void cutString(char* str, int n) {
    int l = strlen(str);
    memmove(str + 0, str + l - n + 1, n);
}

void print_error(char* str) {
    puts(str);
    exit(-1);
}

int main(int argc, char *argv[]) {
    int fd1[2], fd2[2];
    char buffer[buffer_size];
    if (argc != 4) {
        print_error("Incorrect input\n");
    }
    int n = atoi(argv[3]);
    if(n < 1 || n > 127) {
        print_error("Incorrect N\n");
    }
    if (pipe(fd1) < 0) {
        print_error("Can\'t open read pipe\n");
    }

    if (pipe(fd2) < 0) {
        print_error("Can\'t open write pipe\n");
    }

    int pid = fork();
    if(pid < 0){
        print_error("Can\'t create child process :(");
    }
    if (pid == 0) {
        int input = open(argv[1], O_RDONLY, 0666);
        int fsize = read(input, buffer, buffer_size);
        int size = write(fd1[1], buffer, fsize);
    } else {
        int pid2 = fork();
        if (pid2 < 0) {
            print_error("Can\'t create child process :(\n");
        } else if (pid2 == 0) {
            int size = read(fd1[0], buffer, buffer_size);
            char* ans_buf = (char*)malloc(n * sizeof(char));
            
            int b = findSequence(buffer, size, ans_buf, n);
            if (b == -1) {
                n = 9;
                free(ans_buf);
                ans_buf = (char*)malloc(9 * sizeof(char));
                ans_buf = "no answer";
            }
            size = write(fd2[1], ans_buf, n);
        } else {
            int size = read(fd2[0], buffer, buffer_size);
            int file_d = open(argv[2], O_WRONLY | O_TRUNC | O_CREAT, 0666);
            size = write(file_d, buffer, size);
        }
    }
    return 0;
}