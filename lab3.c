#include <stdio.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <wait.h>
#include <errno.h>
#define MAXN 1024
#define LINE 1024
int s1,s2;//信号灯
char (*buf)[LINE];
int *ptr;
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
    if(semop(semid,&sem,1)==-1) {
        printf("P semop error: %d \n",errno);
        exit(EXIT_FAILURE);}
    return ;
}
void V(int semid){
	struct sembuf sem;
	sem.sem_num= 0;
	sem.sem_flg= 0;
	sem.sem_op= 1;
	if(semop(semid,&sem,1)==-1) puts("V semop error");
	return;
}
void sem_init(int semid,int val){
	union semun sem_val;
	sem_val.val= val;
	if(semctl(semid,0,SETVAL,sem_val)<0) printf("semctl initial error\n");
	return;
}
void writebuf(char* fname,int s1,int s2){
    FILE* pf=fopen(fname,"r");
    while(1){
        P(s1);
        ptr[2]=fread(buf[ptr[0]],sizeof(char),LINE,pf);
        if(ptr[2]!=LINE) { V(s2);break;}
        if(++ptr[0]==MAXN) ptr[0]=0;
        V(s2);
    }
    return ;
}
void readbuf(char* fname,int s1,int s2){
    FILE *pf=fopen(fname,"w");
    while(1){
        P(s2);
        if(ptr[1]==ptr[0]){
            fwrite(buf[ptr[1]],sizeof(char),ptr[2],pf);
            V(s1);
            break;
        }
        fwrite(buf[ptr[1]],sizeof(char),LINE,pf);
        if(++ptr[1]==MAXN) ptr[1]=0;
        V(s1);
    }
    return;
}
int main(int argc,char*argv[])
{
    int shmid1=shmget((key_t)3,sizeof(char[LINE])*MAXN,0666|IPC_CREAT);
    int shmid2=shmget((key_t)4,sizeof(int)*3,0666|IPC_CREAT);
    buf=shmat(shmid1,0,0);//share buf
    ptr=(int*)shmat(shmid2,0,0);//ptr[0]->rptr,ptr[1]->wptr,ptr[2]->block count
    ptr[0]=0;//initialize
    ptr[1]=0;
    ptr[2]=0;
	int s1=semget(10,1,IPC_CREAT|0666);
	int s2=semget(20,1,IPC_CREAT|0666);
    sem_init(s1,MAXN-4);
    sem_init(s2,0);
    pid_t p1,p2;p1=fork();
    if(p1==0){
        writebuf(argv[1],s1,s2);
        exit(0);
    }
    else{
        p2=fork();
        if(p2==0){
            readbuf(argv[2],s1,s2);
            exit(0);
        }
    }
    waitpid(p1,NULL,0);
    waitpid(p2,NULL,0);
    shmctl(shmid1,IPC_RMID,0);
    shmctl(shmid2,IPC_RMID,0);
    semctl(s1,0,IPC_RMID);
    semctl(s2,0,IPC_RMID);
    return 0;
}