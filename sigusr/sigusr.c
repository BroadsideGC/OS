#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

void sig_handler(int signo, siginfo_t *siginfo, void *context)
{
  if (signo == SIGUSR1){
    printf("SIGUSR1 from %d\n", (int)siginfo->si_pid);
  }else{
    printf("SIGUSR2 from %d\n", (int)siginfo->si_pid);
  }
  exit(signo);
}

int main(void){
  struct sigaction sa;
  sa.sa_sigaction = *sig_handler;
  if (sigaction(SIGUSR1, &sa, NULL) != 0  || sigaction(SIGUSR2, &sa, NULL) != 0){
    printf("error\n");
     return errno;
  }
  sleep(10);
  printf("No signals were caught\n");
  return 0;
}
