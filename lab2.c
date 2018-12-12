#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <pthread.h>
#include <stdio.h>
union semun
{
  int val;
  struct semid_ds *buf;
  unsigned short *arry;
};
int a=0;//共享变量
int s1,s2;
void P(int semid,int index){
	struct sembuf sem;
    sem.sem_num= index;
    sem.sem_op=-1;
    sem.sem_flg=0;
    semop(semid,&sem,1);
    return ;
}
void V(int semid,int index){
	struct sembuf sem;
	sem.sem_num= index;
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
void *subp1(void *pa){
	for(int i=1;i<=100;i++){
		P(s1,0);
		*(int*)pa+=i;
		V(s2,0);
	}
	return NULL;
}
void *subp2(void *pa){
	for(int i=1;i<=100;i++){
		P(s2,0);
		printf("%d\n",*(int*)pa);
		V(s1,0);
	}
	return NULL;
}

int main()
{
	s1=semget(1,1,IPC_CREAT|0666);
	s2=semget(2,1,IPC_CREAT|0666);
	sem_init(s1,1);
	sem_init(s2,0);
	pthread_t p1,p2;
	pthread_create(&p1,NULL,subp1,&a);
	pthread_create(&p2,NULL,subp2,&a);
	void *ret_val1,*ret_val2;
	pthread_join(p1,&ret_val1);
	pthread_join(p2,&ret_val2);
	return 0;
}

