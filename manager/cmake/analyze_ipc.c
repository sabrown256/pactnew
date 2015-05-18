#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
int main(int c, char **v)
{int pid, st, w;
 pid = fork();
 st  = waitpid(-1, &w, WNOHANG);
 return(1);}
