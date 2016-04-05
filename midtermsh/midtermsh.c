#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>

pid_t childs[1024];
ssize_t child_cnt = 0;

void sig_handler(int signo, siginfo_t *siginfo, void *context) {
    int i;
    for (i = 0; i < child_cnt; i++) {
        kill(childs[i], signo);
        waitpid(childs[i], NULL, 0);
    }
    exit(signo);
}


ssize_t read_buffer(int fd, void *buffer, size_t size) {
    ssize_t total_read_cnt = 0;
    ssize_t read_cnt;
    while (total_read_cnt < size && (read_cnt = read(fd, buffer + total_read_cnt, size - read_cnt)) > 0) {
        total_read_cnt += read_cnt;
        if (*((char *) (buffer + total_read_cnt - 1)) == '\n') {
            break;
        }
    }
    return total_read_cnt;
}

int make_tokens(char *string, char *sep, char *tokens[]) {
    char *token = strtok(string, sep);
    int i = 0;
    while (token) {
        tokens[i++] = token;
        token = strtok(NULL, sep);
    }
    return i;
}

void process(char *commands[], int comm_cnt) {
    char *parts[255];
    int fpipe[2];
    int spipe[2];
    int i;
    for (i = 0; i < comm_cnt; i++) {
        memset(parts, 0, sizeof(parts));
        int parts_cnt = make_tokens(commands[i], " ", parts);
        if (commands[0] == "")
            break;
        parts[parts_cnt] = NULL;
        pipe(spipe);
        pid_t newpid = fork();
        if (newpid < 0) {
            perror("Fork failed\n");
        } else {
            childs[child_cnt++] = newpid;
        }
        if (newpid == 0) {
            if (i > 0) {
                dup2(fpipe[0], STDIN_FILENO);
                close(fpipe[0]);
                close(fpipe[1]);
            }
            if (i != comm_cnt - 1) dup2(spipe[1], STDOUT_FILENO);
            close(spipe[0]);
            close(spipe[1]);
            execvp(parts[0], parts);
        } else {
            if (i > 0) {
                close(fpipe[0]);
                close(fpipe[1]);
            }
            if (i == comm_cnt - 1) {
                close(spipe[0]);
                close(spipe[1]);
            }
        }
        fpipe[0] = spipe[0];
        fpipe[1] = spipe[1];
    }
   wait(NULL);
}

int main() {
    char buffer[4096];
    struct sigaction sa;
    sa.sa_sigaction = &sig_handler;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGINT, &sa, NULL) != 0)
        return errno;
    ssize_t readed = 1;
    while (1) {
        write(1, "> ", 2);
        memset(buffer, 0, sizeof(buffer));
        readed = read_buffer(STDIN_FILENO, buffer, sizeof(buffer));
        if (readed > 0) {
            memset(buffer + readed - 1, 0, sizeof(char));
            char *commands[255];
            int comm_cnt = make_tokens(buffer, "|", commands);
            process(commands, comm_cnt);
        }
    }
}

