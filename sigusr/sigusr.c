#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>

char flag = 0;
void sig_handler(int signo, siginfo_t *siginfo, void *context)
{
  if (flag == 0){
    flag = 1;
    pid_t pid = siginfo->si_pid;
    printf("Signal %d from %d\n", signo, (int) pid);
    exit(signo);
  }
}

int main(void){
  struct sigaction sa;
  sa.sa_sigaction = *sig_handler;
  sa.sa_flags = SA_SIGINFO;
  sigset_t set;
  sigfillset(&set);
  sa.sa_mask = set;
  if (sigaction(SIGUSR1, &sa, NULL) != 0  || sigaction(SIGUSR2, &sa, NULL) != 0){
    printf("error\n");
    return errno;
  }
  sleep(10);
  flag = 1;
  printf("No signals were caught\n");
  return 0;
}
