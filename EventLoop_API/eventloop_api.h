#pragma once

#ifndef __EventLoop_H__
#define __EventLoop_H__

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
};
#endif 