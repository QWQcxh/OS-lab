#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

void perm(long mode){
    if(S_IRUSR &mode) putchar('r');else putchar('-');
    if(S_IWUSR &mode) putchar('w');else putchar('-');
    if(S_IXUSR &mode) putchar('x');else putchar('-');
    if(S_IRGRP &mode) putchar('r');else putchar('-');
    if(S_IWGRP &mode) putchar('w');else putchar('-');
    if(S_IXGRP &mode) putchar('x');else putchar('-');
    if(S_IROTH &mode) putchar('r');else putchar('-');
    if(S_IWOTH &mode) putchar('w');else putchar('-');
    if(S_IXOTH &mode) putchar('x');else putchar('-');
    putchar(' ');
}
void printdir(char *dir, int depth){
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    if  ((dp = opendir(dir))==NULL){     //打开目录
        printf("can't open dir %s\n",dir);
        return ;
    }
    chdir(dir); //进入目录，方便写相对路径
    while((entry=readdir(dp))!=NULL){  //读取一个目录项
        lstat(entry->d_name,&statbuf);
        if(S_ISDIR(statbuf.st_mode)){ //该目录项是目录
            if(strcmp(entry->d_name,".")==0||strcmp(entry->d_name,"..")==0) //.或..跳过
                continue;
            printf("depth:%3d ",depth);
            if(S_ISREG(statbuf.st_mode)) putchar('-');
            else if(S_ISDIR(statbuf.st_mode)) putchar('d');//file type
            perm(statbuf.st_mode);
            printf("%ld ",statbuf.st_nlink);
            
            struct passwd* pw=getpwuid(statbuf.st_uid);
            printf("%5s ",pw->pw_name);
            struct group *grp=getgrgid(statbuf.st_gid);
            printf("%5s ",grp->gr_name);

            printf("%10ld ",statbuf.st_size);

            time_t t = statbuf.st_mtime;
            struct tm lt;
            localtime_r(&t, &lt);
            char timbuf[80];
            strftime(timbuf, sizeof(timbuf), "%m月%d %H:%M", &lt);
            printf("%s ",timbuf);
            printf("%s\n",entry->d_name);
            printdir(entry->d_name,depth+4);
        }
        else{
            printf("depth:%3d ",depth);
            if(S_ISREG(statbuf.st_mode)) putchar('-');
            else if(S_ISDIR(statbuf.st_mode)) putchar('d');//file type
            perm(statbuf.st_mode);
             printf("%ld ",statbuf.st_nlink);
            
            struct passwd* pw=getpwuid(statbuf.st_uid);
            printf("%5s ",pw->pw_name);
            struct group *grp=getgrgid(statbuf.st_gid);
            printf("%5s ",grp->gr_name);

            printf("%10ld ",statbuf.st_size);

            time_t t = statbuf.st_mtime;
            struct tm lt;
            localtime_r(&t, &lt);
            char timbuf[80];
            strftime(timbuf, sizeof(timbuf), "%m月%d %H:%M", &lt);
            printf("%s ",timbuf);
            printf("%s\n",entry->d_name);
        }
    }
    chdir("../");
    closedir(dp);
}

int main(int argc,char* argv[])
{
    if(argc==1) {
        printf("one parameter of path needed!!!\n");
        return 0;
    }
    printdir(argv[1],0);
    return 0;
}
