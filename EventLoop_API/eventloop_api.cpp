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

#define EPOLL_FD_INVALID -1
#define INIT_EVENTS_SIZE 512

static const int EPOLL_MAX_EVENTS = 16;

EventLoop::EventLoop()
    : epoll_fd (EPOLL_FD_INVALID)
    , event_fd(0)
{
}

EventLoop::EventLoop(EventHandlerSharedPtr handler)
    : epoll_fd (EPOLL_FD_INVALID)
    , event_fd(0)
    , mHandler(handler)
{
}

EventLoop::~EventLoop() {
    stop();
}

bool EventLoop::start(){
    printf("start!");
    eventfd_t count;
    // struct epoll_event event, events[EPOLL_MAX_EVENTS];
    epoll_fd = epoll_create1(EPOLL_CLOEXEC);      // 创建 epoll 对象
    event_fd = eventfd(0, EFD_NONBLOCK | EFD_SEMAPHORE);
    std::thread worker1(&EventLoop::worker, this);
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
            if (events[i].events == EPOLLERR){
                printf("fd%d Error\n", i);
                return false;
            }
            if (events[i].events == EPOLLHUP){
                printf("event_fd%d Hang up\n", i);
                return false;
            }
            if (events[i].events == EPOLLOUT){
                printf("event_fd%d can write\n", i);
                return false;
            }
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
    // printf("stop!");
    // int efd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    // printf("worker thread start..., efd = %d\n", efd);
    int number = 1;
    int size = eventfd_write(event_fd, (eventfd_t)number);
    printf("size = %d, errno = %d\n", size, errno);
    // for (int i = 1; i < 10; i++) {
    //     int size = eventfd_write(efd, (eventfd_t)number);
    //     printf("i = %d, size = %d, errno = %d\n", i, size, errno);
    // }
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

void  EventLoop::worker(int p_data) {
    int* event_fd = &p_data;
    printf("worker thread start..., event_fd = %d\n", *event_fd);
    // sleep(1);
    return ;
}

EventHandler &EventHandler::setOnRegistered(EventHandler::HandlerFunc func) {
    mOnRegistered = func;
    return *this;
}

EventHandler &EventHandler::setOnUnRegistered(HandlerFunc func) {
    mOnUnRegistered = func;
    return *this;
}

EventHandler &EventHandler::setOnEpollIn(HandlerFunc func) {
    mOnEpollIn = func;
    return *this;
}

EventHandler &EventHandler::setOnEpollOut(HandlerFunc func) {
    mOnEpollOut = func;
    return *this;
}

EventHandler &EventHandler::setOnEpollHup(HandlerFunc func) {
    mOnEpollHup = func;
    return *this;
}

EventHandler &EventHandler::setOnEpollError(HandlerFunc func) {
    mOnEpollError = func;
    return *this;
}

void EventHandler::onRegistered(int fd) {
    if (mOnRegistered != nullptr) {
        mOnRegistered(fd);
    }
}

void EventHandler::onUnRegistered(int fd) {
    if (mOnUnRegistered != nullptr) {
        mOnUnRegistered(fd);
    }
}

void EventHandler::onEpollIn(int fd) {
    if (mOnEpollIn != nullptr) {
        mOnEpollIn(fd);
    }
}

void EventHandler::onEpollOut(int fd) {
    if (mOnEpollOut != nullptr) {
        mOnEpollOut(fd);
    }
}

void EventHandler::onEpollHup(int fd) {
    if (mOnEpollHup != nullptr) {
        mOnEpollHup(fd);
    }
}

void EventHandler::onEpollError(int fd) {
    if (mOnEpollError != nullptr) {
        mOnEpollError(fd);
    }
}