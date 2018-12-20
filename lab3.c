#include <stdio.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <wait.h>
#define MAXN 1024
int s1,s2;//信号灯
int *buf,*ptr;
union semun
{
  int val;
  struct semid_ds *buf;
  unsigned short *arry;
};
void P(int semid){
	struct sembuf sem;
    sem.sem_num= 0;
    sem.sem_op=-1;
    sem.sem_flg=0;
    semop(semid,&sem,1);
    return ;
}
void V(int semid){
	struct sembuf sem;
	sem.sem_num= 0;
	sem.sem_flg= 0;
	sem.sem_op= 1;
	semop(semid,&sem,1);
	return;
}
void sem_init(int semid,int val){
	union semun sem_val;
	sem_val.val= val;
	semctl(semid,0,SETVAL,sem_val);
	return;
}
void writebuf(char *fname){
    freopen(fname,"r",stdin);
    freopen("mdzz","w",stdout);//debug
    int c;
    while(1){
        P(s1);
        printf("ptr[1] is %d\n",ptr[1]);
        while((ptr[0]+1)%MAXN!=ptr[1]){
            c=getchar();
            if(c==EOF) { V(s2); break;}
            buf[ptr[0]]=c;
            printf("%d\n",ptr[0]);
            ptr[0]=(ptr[0]+1)%MAXN;
        }
        printf("one round\n");
        V(s2);
        if(c==EOF) break;
    }
    ptr[2]=1;
    return ;
}
void readbuf(char* fname){
    freopen(fname,"w",stdout);
    while(1){
        P(s2);
        while((ptr[1]+1)%MAXN!=ptr[0]){
            ptr[1]=(ptr[1]+1)%MAXN;
            printf("%c",buf[ptr[1]]);
        }
        V(s1);
        if(ptr[2]) break;
    }
    return ;
}
int main(int argc,char*argv[])
{
    int shmid1=shmget((key_t)3,sizeof(char)*MAXN,0666|IPC_CREAT);
    int shmid2=shmget((key_t)4,sizeof(int)*3,0666|IPC_CREAT);
    buf=(int*)shmat(shmid1,0,0);//share buf
    ptr=(int*)shmat(shmid2,0,0);//ptr[0]->rptr,ptr[1]->wptr,ptr[2]->finish flag
    ptr[0]=1;//initialize
    ptr[1]=0;
    ptr[2]=0;
	int s1=semget(1,1,IPC_CREAT|0666);
	int s2=semget(2,1,IPC_CREAT|0666);
    sem_init(s1,1);
    sem_init(s2,0);
    pid_t p1,p2;p1=fork();
    if(p1==0){
        writebuf(argv[1]);
        exit(0);
    }
    else{
        p2=fork();
        if(p2==0){
            readbuf(argv[2]);
            exit(0);
        }
    }
    waitpid(p1,NULL,0);
    waitpid(p2,NULL,0);
    shmctl(shmid1,IPC_RMID,0);
    shmctl(shmid2,IPC_RMID,0);
    return 0;
}