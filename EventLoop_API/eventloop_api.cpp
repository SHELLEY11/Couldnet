#include "eventloop_api.h"
// #include <iostream>
// using namespace std;

bool EventLoop::start(){
    printf("start!");
    return true;
}

void EventLoop::stop(){
    printf("stop!");
    return;
}

bool EventLoop::registered(int fd, int op){
    if (fd > 0){
        printf("registered: fd = %d, op = %d\n", fd, op);
        return true;
    }
    return false;
}

bool EventLoop::unRegistered(int fd){
    if (fd < 0){
        printf("unRegistered: fd = %d\n", fd);
    }
    return false;
}