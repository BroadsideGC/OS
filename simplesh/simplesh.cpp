#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>
#include <string>
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

vector<pid_t> childs;


void sig_handler(int signo, siginfo_t *siginfo, void *context) {
    for (auto i = 0; i < childs.size(); i++) {
        kill(childs[i], signo);
        waitpid(childs[i], NULL, 0);
    }
    write(STDOUT_FILENO, "\n", 1);
}

size_t BUFFER_SIZE = 8192;

string read_string() {
    string s = "";
    char buffer[BUFFER_SIZE];
    ssize_t total_read_cnt = 0;
    ssize_t read_cnt;

    while ((read_cnt = read(STDIN_FILENO, buffer, sizeof(buffer))) > 0) {
        total_read_cnt += read_cnt;
        if (*(buffer + total_read_cnt - 1) == '\n') {
            break;
        }
    }

    if (total_read_cnt == 0 && read_cnt == 0) {
        raise(SIGINT);
        exit(0);
    }

    for (auto i = 0; i < total_read_cnt; i++) {
        if (buffer[i] == '\n') {
            break;
        }
        s += string{buffer[i]};
    }
    return s;
}

vector<string> make_tokens(char *str, char *sep) {
    auto *token = strtok(str, sep);
    vector<string> tokens;
    while (token) {
        tokens.push_back(token);
        token = strtok(NULL, sep);
    }
    return tokens;
}

void process(vector<string> &commands) {
    int fpipe[2];
    int spipe[2];
    for (auto i = 0; i < commands.size(); i++) {
        auto parts = make_tokens(const_cast<char *>(commands[i].c_str()), (char *) " ");
        if (commands[0] == "")
            break;
        pipe(spipe);
        auto newpid = fork();
        if (newpid < 0) {
            perror("Fork failed\n");
        } else {
            childs.push_back(newpid);
        }
        if (newpid == 0) {
            if (i > 0) {
                dup2(fpipe[0], STDIN_FILENO);
                close(fpipe[0]);
                close(fpipe[1]);
            }
            if (i != commands.size() - 1) dup2(spipe[1], STDOUT_FILENO);
            close(spipe[0]);
            close(spipe[1]);
            vector<const char *> args(parts.size());
            transform(parts.begin(), parts.end(), args.begin(), mem_fun_ref(&string::c_str));
            args.push_back(NULL);
            if (execvp(args[0], (char *const *) args.data()) != 0) {
                childs.pop_back();
                write(STDOUT_FILENO, "Command not found", 17);
                raise(SIGINT);
                exit(errno);
            }
        } else {
            if (i > 0) {
                close(fpipe[0]);
                close(fpipe[1]);
            }
            if (i == commands.size() - 1) {
                close(spipe[0]);
                close(spipe[1]);
            }
        }
        fpipe[0] = spipe[0];
        fpipe[1] = spipe[1];
    }
    for (auto i = 0; i < childs.size(); i++) {
        waitpid(childs[i], 0, 0);
    }
}

int main(void) {
    struct sigaction sa;
    sa.sa_sigaction = &sig_handler;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGINT, &sa, NULL) != 0)
        return errno;
    while (1) {
        write(STDOUT_FILENO, "$ ", 2);
        string str = read_string();
        vector<string> commands = make_tokens(const_cast<char *>(str.c_str()), (char *) "|");
        process(commands);
    }
}