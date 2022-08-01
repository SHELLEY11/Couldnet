#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/eventfd.h>
#include <signal.h>
#include <sys/epoll.h>
 
static const int EPOLL_MAX_EVENTS = 16;
 
void* worker(void* p_data) {
    int* event_fd = (int *)p_data;
    printf("worker thread start..., event_fd = %d\n", *event_fd);
    int number = 1;
    for (int i = 1; i < 10; i++) {
        int size = eventfd_write(*event_fd, (eventfd_t)number);
        printf("i = %d, size = %d, errno = %d\n", i, size, errno);
    }
    printf("worker thread stop...\n");
    // sleep(1);
    return NULL;
}

int main() {
    eventfd_t count;
    struct epoll_event event, events[EPOLL_MAX_EVENTS];
    pthread_t p_main = pthread_self();
    int epoll_fd = epoll_create1(EPOLL_CLOEXEC);      // 创建 epoll 对象

    int event_fd = eventfd(0, EFD_NONBLOCK | EFD_SEMAPHORE);
    printf("event_fd = %d\n", event_fd);
    event.data.fd = event_fd;
    event.events = EPOLLIN;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, event_fd, &event);   // 监听可读事件
 
    pthread_t p_worker;
    pthread_create(&p_worker, NULL, worker, &event_fd);
 
    for (;;) {
        // printf("main thread wait events...\n");
        // 开始等待事件发生
        int len = epoll_wait(epoll_fd, events, EPOLL_MAX_EVENTS, -1);
        // printf("len of epoll_wait = %d\n", len);
        // 开始处理事件
        for (int i = 0; i < len; i++) {
            if (events[i].events == EPOLLIN) {
                eventfd_read(event_fd, &count);
                printf("count = %d\n", count);
            }
        }
    }
 
   return 0;
}