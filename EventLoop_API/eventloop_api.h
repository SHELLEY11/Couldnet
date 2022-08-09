#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/eventfd.h>
#include <signal.h>
#include <sys/epoll.h>
#include <stdint.h>
#include <sys/time.h>
#include <thread>

#ifndef __EventLoop_H__
#define __EventLoop_H__

class EventHandler;
using EventHandlerSharedPtr = std::shared_ptr<EventHandler>;

class EventHandler{
public:
    using HandlerFunc = std::function<void(int)>;
    EventHandler& instance() { return *this; };
    EventHandler& setOnRegistered(HandlerFunc func);
    EventHandler& setOnUnRegistered(HandlerFunc func);
    EventHandler& setOnEpollIn(HandlerFunc func);
    EventHandler& setOnEpollOut(HandlerFunc func);
    EventHandler& setOnEpollHup(HandlerFunc func);
    EventHandler& setOnEpollError(HandlerFunc func);
    void onRegistered(int fd);
    void onUnRegistered(int fd);
    void onEpollIn(int fd);
    void onEpollOut(int fd);
    void onEpollHup(int fd);
    void onEpollError(int fd);
private:
    HandlerFunc mOnRegistered;
    HandlerFunc mOnUnRegistered;
    HandlerFunc mOnEpollIn;
    HandlerFunc mOnEpollOut;
    HandlerFunc mOnEpollHup;
    HandlerFunc mOnEpollError;
};

class EventLoop { 
    public:   
    /**    
    * @brief EventLoop    
    */   
    EventLoop();   
    ~EventLoop();   
    /**    
    * @brief EventLoop    
    * @param handler    
    */   
    explicit EventLoop(EventHandlerSharedPtr handler);   
    /**    
    * @brief start    
    * @return    
    */   
    bool start();   
    /**    
    * @brief stop    
    */   
    void stop();   
    /**    
    * @brief registered    
    * @param fd    
    * @param op    
    * @return    
    */   
    bool registered(int fd, int op);   
    /**    
    * @brief unRegistered    
    * @param fd    
    * @return    
    */   
    bool unRegistered(int fd);
private:
    struct epoll_event event, events[EPOLL_MAX_EVENTS];
    int event_fd ;
    int epoll_fd ;
    EventHandlerSharedPtr mHandler;
};
#endif 