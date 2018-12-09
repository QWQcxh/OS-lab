#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

int child_num=0;
int fd[2];//file descriptor
pid_t p1=-1,p2=-1;
void child_func(int signum){
    if(signum==SIGUSR1){
        close(fd[0]);
        close(fd[1]);
        printf("Child Process %d is Killed by Parent!\n",child_num);
        exit(0);
    }
}
void parent_func(int signum){
    if(signum==SIGINT){
        kill(p1,SIGUSR1);
        kill(p2,SIGUSR1);
    }
}
int main()
{
    pipe(fd);//create anonymous pipe
    if(signal(SIGINT,parent_func)==SIG_ERR){
        printf("SIGINT can't be caught.\n");
    }
    p1=fork();
    if(p1==0) {//child process 1
        int x=1;
        char w_buf[80];
        child_num=1;
        if(signal(SIGUSR1,child_func)==SIG_ERR){
            printf("SIGUSR1 can't be caught.\n");
        }
        if(signal(SIGINT,SIG_IGN)==SIG_ERR){
            printf("SIGINT can't be caught.\n");
        }
        while(1){
            sprintf(w_buf,"I send you %d times.",x++);
            write(fd[1],w_buf,sizeof(w_buf));
            sleep(1);
        }
    }
    else{ //father process
        p2=fork();
        if(p2==0){ //p2 process
            char r_buf[80];
            child_num=2;
            if(signal(SIGUSR1,child_func)==SIG_ERR){
                printf("SIGUSR1 can't be caught.\n");
            }
            if(signal(SIGINT,SIG_IGN)==SIG_ERR){
                printf("SIGINT can't be caught.\n");
            }
            while(1){
                read(fd[0],r_buf,sizeof(r_buf));
                puts(r_buf);
            }
        }
    }
    waitpid(p1,NULL,0);
    waitpid(p2,NULL,0);
    close(fd[0]);
    close(fd[1]);
    printf("Parent Process is Killed!.\n");
    return 0;
}