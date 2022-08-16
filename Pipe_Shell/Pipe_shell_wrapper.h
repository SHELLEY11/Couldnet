#ifndef _PipeShell_Wrapper_H__
#define _PipeShell_Wrapper_H__

#ifdef __cplusplus
extern "C" {
#endif

extern int PipeShellInstance(int * handle);
extern void ReleaseInstance(int *handle);
extern  int get_input(char buf[]);
extern void parse(char* buf);
extern int callCd(int argc);
extern int commandWithPipe(char buf[]) ;
extern int commandInBackground(char buf[]); 
extern void do_cmd(int argc, char* argv[]) ;


#ifdef __cplusplus
};
#endif

#endif