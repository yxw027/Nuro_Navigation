#include "./NuroDefine.h"

/*
nuHANDLE nuCreateThread (PNURO_SECURITY_ATTRIBUTES lpsa, 
						 nuDWORD cbStack, 
						 LPNURO_THREAD_START_ROUTINE lpStartAddr,
						 nuPVOID lpvThreadParam,
						 nuDWORD fdwCreate,
						 nuDWORD *lpIDThread);
*/

//typedef nuDWORD (*LPNURO_THREAD_START_ROUTINE)(nuPVOID arg_addr);

#include <stdio.h>
nuDWORD run(nuPVOID param)
{
    printf("dwdw.dw.dw.\n");
    return 100;
}

int main()
{
    nuHANDLE handle = nuCreateThread(NULL, 0, run, NULL, 0, NULL);
    
    nuDWORD n = nuThreadWaitReturn(handle);

    printf("%ld\n", n);
    
    return 0;
}

