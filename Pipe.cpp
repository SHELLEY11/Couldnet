#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/wait.h>
 
/*读管道*/
void read_from_pipe(int fd)
{
    char message[10000];
    memset(message, 0, 10000);
    read(fd,message,10000);
    if (!message) {
      printf("read none\n");
    }
    printf("read from pipe:%s\n",message);
}
 
/*写管道*/
void write_to_pipe(int fd)
{
    const  char *message="Hello,pipe!\n";
    write(fd,message,strlen(message)+1);
}

int main(void){

    int fd[2];
    pid_t pid;
    int stat_val;
    char* arglist[] = {"ls","-l","-a",NULL};
    if(pipe(fd))
    {
       printf("create pipe failed!\n");
       exit(1);
    }  
     
    pid=fork();  // 创建子进程
    switch(pid)
    {
       case -1:
          printf("fork error!\n");
          exit(1);
       case 0:
          //子进程
          dup2(fd[1], STDOUT_FILENO);
          close(fd[0]);//关闭子进程读端
          // write_to_pipe(fd[1]);//子进程写
          execvp(arglist[0], arglist);    // shell
          //system("ls");
          exit(0);
       default:
          //父进程
          close(fd[1]);//关闭父进程写端
          //read_from_pipe(fd[0]);//父进程读
         int i = 0;
          while(arglist[i]){
            read_from_pipe(fd[0]);
            i++;
          }
          wait (&stat_val); //等待子进程
          exit(0);
    }
    return 0;
}
