#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

ssize_t fprint(ssize_t fd) {
    char buff[2048];
    ssize_t cnt_r;
    while ((cnt_r = read(fd, buff, sizeof(buff))) != 0) {
        ssize_t cnt_w = 0;
        if (cnt_r < 0){
          if (errno == EINTR) {
              continue;
            }
            perror("Error during reading");
            return -1;
        }
        while (cnt_w != cnt_r) {
            ssize_t cnt_cw = write(STDOUT_FILENO, buff + cnt_w, cnt_r - cnt_w);
            if (cnt_cw < 0) {
                if (errno == EINTR) {
                    continue;
                }
                perror("Error during writing");
                return -1;
            }
            cnt_w += cnt_cw;
        }
    }
    if (cnt_r != 0) {
        return -1;
    }
    return 0;
}


int main(int argc, char **argv) {
    if (argc < 2) {
        if (fprint(STDIN_FILENO) != 0) {
            perror("Error during reading");
            return -1;
        }
        return 0;
    }
    for (int i = 1; i < argc; i++) {
        ssize_t fd;
        if ((fd = open(argv[i], O_RDONLY)) == -1) {
            perror("Can`t open file");
            return -1;
        }
        if (fprint(fd) != 0) {
            perror("Error during reading or writing");
            return -1;
        }
        if (close(fd) != 0) {
            perror("Error during closing");
            return -1;
        }
    }
    return 0;
}
