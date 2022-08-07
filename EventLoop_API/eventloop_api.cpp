#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/eventfd.h>
#include <signal.h>
#include <sys/epoll.h>
#include <stdint.h>
#include <sys/time.h>
#include <thread>
#include "eventloop_api.h"
// #include <iostream>
// using namespace std;
static const int EPOLL_MAX_EVENTS = 16;
struct epoll_event event, events[EPOLL_MAX_EVENTS];
int event_fd = eventfd(0, EFD_NONBLOCK | EFD_SEMAPHORE);
struct EventHandlerSharedPtr{
    
};
EventLoop::EventLoop(EventHandlerSharedPtr handler){
    
}

bool EventLoop::start(){
    printf("start!");
    eventfd_t count;
    // struct epoll_event event, events[EPOLL_MAX_EVENTS];
    int epoll_fd = epoll_create1(EPOLL_CLOEXEC);      // 创建 epoll 对象
    // int event_fd = eventfd(0, EFD_NONBLOCK | EFD_SEMAPHORE);
        for (;;) {
        // printf("main thread wait events...\n");
        // 开始等待事件发生
        int len = epoll_wait(epoll_fd, events, EPOLL_MAX_EVENTS, -1);
        // printf("len of epoll_wait = %d\n", len);
        // 开始处理事件
        if (len == 0) {
            /* time out */
            printf("epoll wait timed out.\n");
            break;
        }
        if (len < 0){
            perror("epoll wait error:");
            return false;
        }
        for (int i = 0; i < len; i++) {
            if (events[i].events == EPOLLIN) {
                eventfd_read(event_fd, &count);
                if (eventfd_read(event_fd, &count) < 0){
                    printf("eventfd read failed:");
                    return -1;
                }
                printf("count = %d\n", count);
            }
        }
    }
    return true;
}

void EventLoop::stop(){
    printf("stop!");
    int efd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    printf("worker thread start..., efd = %d\n", efd);
    int number = 1;
    for (int i = 1; i < 10; i++) {
        int size = eventfd_write(efd, (eventfd_t)number);
        printf("i = %d, size = %d, errno = %d\n", i, size, errno);
    }
    printf("worker thread stop...\n");
    return;
}

bool EventLoop::registered(int fd, int op){
    
    if (epoll_ctl(fd,op,event_fd, &event) < 0){
        printf("event add failed [fd=%d], events[%d]\n", event_fd, events);
        return false;
    } else {
        printf("event add OK [fd=%d], op=%d, events[%OX]\n", event_fd, op,events);
        return true;
    }
    
}

bool EventLoop::unRegistered(int fd){
    if (fd < 0){
        epoll_ctl(fd, EPOLL_CTL_DEL, event_fd, &event);
        close(event_fd);
        return true;
    }
    return false;
}