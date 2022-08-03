#ifndef _Eventloop_Wrapper_H__
#define _Eventloop_Wrapper_H__

#ifdef __cplusplus
extern "C" {
#endif

extern int EventLoopInstance(int * handle);
extern void ReleaseInstance(int *handle);
extern bool start();
extern void stop();
extern bool registered(int handle, int op);
extern bool unRegistered(int handle);

#ifdef __cplusplus
};
#endif

#endif