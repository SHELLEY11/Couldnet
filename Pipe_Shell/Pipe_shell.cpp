#include"Pipe_shell.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<signal.h>
#include<dirent.h>
#include<fcntl.h>
#include<ctype.h>

#define IN 1
#define OUT 0
#define MAX_CMD 10
#define BUFFSIZE 100
#define MAX_CMD_LEN 100

PipeShell::PipeShell(){
    command = {'0'};
    backupBuf = {'0'};
    curPath = {'0'};
    i = 0,j = 0;
}

PipeShell::~PipeShell(){
    
}

/* get_input接受输入的字符并存入buf数组中 */
int PipeShell::get_input(char buf[]) {
    // buf数组初始化
    memset(buf, 0x00, BUFFSIZE);
    memset(backupBuf, 0x00, BUFFSIZE);        

    fgets(buf, BUFFSIZE, stdin);
    // 去除fgets带来的末尾\n字符
    buf[strlen(buf) - 1] = '\0';
    return strlen(buf);
}

void PipeShell::parse(char* buf) {
    // 初始化argv和command数组
    for (i = 0; i < MAX_CMD; i++) {
        argv[i] = NULL;
        for (j = 0; j < MAX_CMD_LEN; j++)
            command[i][j] = '\0';
    }
    argc = 0;
    // 下列操作改变了buf数组, 为buf数组做个备份
    strcpy(backupBuf, buf);
    /** 构建command数组
     *  即若输入为"ls -a"
     *  strcmp(command[0], "ls") == 0 成立且
     *  strcmp(command[1], "-a") == 0 成立
     */  
    int len = strlen(buf);
    for (i = 0, j = 0; i < len; ++i) {
        if (buf[i] != ' ') {
            command[argc][j++] = buf[i];
        } else {
            if (j != 0) {
                command[argc][j] = '\0';
                ++argc;
                j = 0;
            }
        }
    }
    if (j != 0) {
        command[argc][j] = '\0';
    }

    /** 构建argv数组
     *  即若输入buf为"ls -a"
     *  strcmp(argv[0], "ls") == 0 成立且
     *  strcmp(argv[1], "-a") == 0 成立*/
    argc = 0;
    int flg = OUT;
    for (i = 0; buf[i] != '\0'; i++) {
        if (flg == OUT && !isspace(buf[i])) {
            flg = IN;
            argv[argc++] = buf + i;
        } else if (flg == IN && isspace(buf[i])) {
            flg = OUT;
            buf[i] = '\0';
        }
    }
    argv[argc] = NULL;
}

int PipeShell::commandWithPipe(char buf[BUFFSIZE]) {
    // 获取管道符号的位置索引
    for(j = 0; buf[j] != '\0'; j++) {
        if (buf[j] == '|')
            break;
    }

    // 分离指令, 将管道符号前后的指令存放在两个数组中
    // outputBuf存放管道前的命令, inputBuf存放管道后的命令
    char outputBuf[j];
    memset(outputBuf, 0x00, j);
    char inputBuf[strlen(buf) - j];
    memset(inputBuf, 0x00, strlen(buf) - j);
    for (i = 0; i < j - 1; i++) {
        outputBuf[i] = buf[i];
    }
    for (i = 0; i < strlen(buf) - j - 1; i++) {
        inputBuf[i] = buf[j + 2 + i];
    }


    int pd[2];
    pid_t pid;
    if (pipe(pd) < 0) {
        perror("pipe()");
        exit(1);
    }

    pid = fork();
    if (pid < 0) {
        perror("fork()");
        exit(1);
    }
    if (pid == 0) {                     // 子进程写管道
        close(pd[0]);                   // 关闭子进程的读端
        dup2(pd[1], STDOUT_FILENO);     // 将子进程的写端作为标准输出
        parse(outputBuf);
        execvp(argv[0], argv);
        if (pd[1] != STDOUT_FILENO) {
            close(pd[1]);
        }
    }else {                              // 父进程读管道
        /** 关键代码
         *  子进程写管道完毕后再执行父进程读管道, 
         *  所以需要用wait函数等待子进程返回后再操作
         */
        int status;
        waitpid(pid, &status, 0);       // 等待子进程返回
        int err = WEXITSTATUS(status);  // 读取子进程的返回码
        if (err) { 
            printf("Error: %s\n", strerror(err));
        }
        close(pd[1]);                    // 关闭父进程管道的写端
        dup2(pd[0], STDIN_FILENO);       // 管道读端读到的重定向为标准输入
        parse(inputBuf);
        execvp(argv[0], argv);
        if (pd[0] != STDIN_FILENO) {
            close(pd[0]);
        }       
    }
    return 1;
}

int PipeShell::callCd(int argc) {
    // result为1代表执行成功, 为0代表执行失败
    int result = 1;
    if (argc != 2) {
        printf("指令数目错误!");
    } else {
        int ret = chdir(command[1]);
        if (ret) return 0;
    }

    if (result) {
        char* res = getcwd(curPath, BUFFSIZE);
        if (res == NULL) {
            printf("文件路径不存在!");
        }
        return result;
    }
    return 0;
}

int PipeShell::commandInBackground(char buf[BUFFSIZE]) {
    char backgroundBuf[strlen(buf)];
    memset(backgroundBuf, 0x00, strlen(buf));
    for (i = 0; i < strlen(buf); i++) {
        backgroundBuf[i] = buf[i];
        if (buf[i] == '&') {
            backgroundBuf[i] = '\0';
            backgroundBuf[i - 1] = '\0';
            break;
        }
    }

    pid_t pid;
    pid = fork();
    if (pid < 0) {
        perror("fork()");
        exit(1);
    }

    if (pid == 0) {
        // 将stdin、stdout、stderr重定向到/dev/null
        freopen( "/dev/null", "w", stdout );
        freopen( "/dev/null", "r", stdin ); 
        signal(SIGCHLD,SIG_IGN);
        parse(backgroundBuf);
        execvp(argv[0], argv);
        printf("%s: 命令输入错误\n", argv[0]);
        // exit函数终止当前进程, 括号内参数为1时, 会像操作系统报告该进程因异常而终止
        exit(1);
    }else {
        exit(0);
    }
}

void PipeShell::do_cmd(int argc, char* argv[]) {
    pid_t pid;
    /* 识别program命令 */
    
    // 识别管道命令
    for (j = 0; j < MAX_CMD; j++) {
        if (strcmp(command[j], "|") == 0) {
            strcpy(buf, backupBuf);
            int sample = commandWithPipe(buf);
            return;
        }
    }

    // 识别后台运行命令
    for (j = 0; j < MAX_CMD; j++) {
        if (strcmp(command[j], "&") == 0) {
            strcpy(buf, backupBuf);
            int sample = commandInBackground(buf);
            return;
        }
    }

    /* 识别shell内置命令 */
    if (strcmp(command[0], "cd") == 0) {
        int res = callCd(argc);
        if (!res) printf("cd指令输入错误!");
    } 
    // else if (strcmp(command[0], "history") == 0) {
    //     printHistory(command);
    // } 
    else if (strcmp(command[0], "exit") == 0) {
        exit(0);
    } else {
        switch(pid = fork()) {
            // fork子进程失败
            case -1:
                printf("创建子进程未成功");
                return;
            // 处理子进程
            case 0:
                {   /* 函数说明：execvp()会从PATH 环境变量所指的目录中查找符合参数file 的文件名, 找到后便执行该文件, 
                     * 然后将第二个参数argv 传给该欲执行的文件。
                     * 返回值：如果执行成功则函数不会返回, 执行失败则直接返回-1, 失败原因存于errno 中. 
                     * */
                    execvp(argv[0], argv);
                    // 代码健壮性: 如果子进程未被成功执行, 则报错
                    printf("%s: 命令输入错误\n", argv[0]);
                    // exit函数终止当前进程, 括号内参数为1时, 会像操作系统报告该进程因异常而终止
                    exit(1);
                }
            default: {
                    int status;
                    waitpid(pid, &status, 0);      // 等待子进程返回
                    int err = WEXITSTATUS(status); // 读取子进程的返回码

                    if (err) { 
                        printf("Error: %s\n", strerror(err));
                    }                    
            }
        }
    }
}