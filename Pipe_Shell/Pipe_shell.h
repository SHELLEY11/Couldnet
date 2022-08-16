#pragma once

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

#ifndef __ PipeShell_H__
#define __PipeShell_H__

class PipeShell{
    private:
        char command[MAX_CMD][MAX_CMD_LEN];             // 参数数组
        char backupBuf[BUFFSIZE];                       // 参数数组的备份
        char curPath[BUFFSIZE];                         // 当前shell工作路径
        int i, j;                                       // 循环变量
    public:
        int argc;                                       // 有效参数个数
        char* argv[MAX_CMD];                            // 参数数组
        char buf[BUFFSIZE];                             // 接受输入的参数数组

    PipeShell();
    ~PipeShell();

    int get_input(char buf[]);
    void parse(char* buf);
    int callCd(int argc);
    int commandWithPipe(char buf[]) ;
    int commandInBackground(char buf[]); 
    void do_cmd(int argc, char* argv[]) ;
};
#endif 