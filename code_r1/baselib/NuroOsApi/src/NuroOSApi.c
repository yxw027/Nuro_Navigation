#include "../NuroOSApi.h"
#include "NuroDefine.h"

//#define DAIWOO 
#define _ABS(n) ((n) > 0 ? (n) : (-(n)))
#define _MIN(a, b) (((a) < (b)) ? (a) : (b))
#define _MAX(a, b) (((a) > (b)) ? (a) : (b))

#define _LOWORD(l)           ((nuWORD)(l))
#define _HIWORD(l)           ((nuWORD)(((nuDWORD)(l) >> 16) & 0xFFFF))

#ifdef NURO_OS_WINDOWS
#define _CRT_INSECURE_DEPRECATE(...)
#include <stdio.h>
#ifdef WINCE
#define _beginthreadex CreateThread
#else
#include <process.h>
#endif
#include <windows.h>
#endif

#ifdef NURO_OS_WINDOWS
#ifndef WINCE
#include <Mmsystem.h>
#pragma comment(lib, "Winmm.lib")
#endif
#endif

#ifdef NURO_OS_WINDOWS
#define _USE_DATA_MEMORY_OF_FILE_MAPPING
#endif

#ifdef NURO_OS_LINUX
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <dlfcn.h>
#include <pthread.h>

#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <signal.h>
#include <stdbool.h>

nuINT  m_fd = -1;
#define BUF_LENGTH 1024
nuBYTE  ReceiveCharBuf[BUF_LENGTH] = {0}; 
nuCHAR	g_path[256] = {0};

nuINT speed_arr[] = { B38400, B19200, B9600, B4800, B2400, B1200, B300,
    		      B38400, B19200, B9600, B4800, B2400, B1200, B300, };
nuINT name_arr[] =  { 38400,  19200,  9600,  4800,  2400,  1200,  300, 38400,  
    	   	      19200,  9600, 4800, 2400, 1200,  300, };

static nuINT _nRate = 38400;
#define UARTDEV			  "/dev/ttymxc1"
#define reOpenUARTDEV 	          "/dev/block/vold/ttyUSB0"
#endif

#define ANDROID_PATH  "/mnt/sdcard/nurownds/" ////mnt/sdcard/extsd/NuroWnds/

#ifdef NURO_OS_LINUX
nuVOID set_speed(nuINT fd, nuINT speed)
{
    nuINT   i; 
    nuINT   status; 
    struct termios   Opt;

    tcgetattr(fd, &Opt); 
    //Opt_bk = Opt;

    for (i= 0; i < (nuINT)sizeof(speed_arr) / (nuINT)sizeof(nuINT); i++) 
    {
        if  (speed == name_arr[i]) 
        {
            tcflush(fd, TCIOFLUSH);     
            cfsetispeed(&Opt, speed_arr[i]);  
            cfsetospeed(&Opt, speed_arr[i]);   
            status = tcsetattr(fd, TCSANOW, &Opt);  
            if  (status != 0)
            {
                //perror("tcsetattr fd");  
		////__android_log_print((ANDROID_LOG_INFO, "usb", "open !=0");
                return;     
            }    
            tcflush(fd,TCIOFLUSH);   
        }  
    }
}

/**
 * *@param  fd     类型  int  打开的串口文件句柄
 * *@param  databits 类型  int 数据位   取值 为 7 或者8
 * *@param  stopbits 类型  int 停止位   取值为 1 或者2
 * *@param  parity  类型  int  效验类型 取值为N,E,O,,S
 * */
nuINT set_Parity(nuINT fd,nuINT databits,nuINT stopbits,nuINT parity)
{ 
    struct termios options; 
    if (tcgetattr(fd,&options) != 0)
    { 
        //perror("SetupSerial");
	////__android_log_print((ANDROID_LOG_INFO, "usb", "set_Parity tcgetattr fail");     
        return(false);  
    }
    options.c_cflag &= ~CSIZE; 
    switch (databits) /*设置数据位数*/
    {   
        case 7:     
            options.c_cflag |= CS7; 
            break;
        case 8:     
            options.c_cflag |= CS8;
            break;   
        default:    
            fprintf(stderr,"Unsupported data size\n"); return (false);  
    }
    switch (parity) 
    {   
        case 'n':
        case 'N':    
            options.c_cflag &= ~PARENB;  
            options.c_iflag &= ~INPCK;  
            break;  
        case 'o':   
        case 'O':     
            options.c_cflag |= (PARODD | PARENB);
            options.c_iflag |= INPCK;
            break;  
        case 'e':  
        case 'E':   
            options.c_cflag |= PARENB;
            options.c_cflag &= ~PARODD;
            options.c_iflag |= INPCK;
            break;
        case 'S': 
        case 's': 
            options.c_cflag &= ~PARENB;
            options.c_cflag &= ~CSTOPB;break;  
        default:   
            fprintf(stderr,"Unsupported parity\n");    
            return (false);  
    }  
    /* 设置停止位*/  
    switch (stopbits)
    {   
        case 1:    
            options.c_cflag &= ~CSTOPB;  
            break;  
        case 2:    
            options.c_cflag |= CSTOPB;  
            break;
        default:    
            fprintf(stderr,"Unsupported stop bits\n");  
            return (false); 
    } 
    
    if (parity != 'n')   
    {
        options.c_iflag |= INPCK;
    }

    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tcflush(fd,TCIFLUSH);
    options.c_cc[VTIME] = 10; /* 设置超时15 seconds*/   
    options.c_cc[VMIN] = 0; 
    if (tcsetattr(fd,TCSANOW,&options) != 0)   
    { 
        //perror("SetupSerial 3");   
        return (false);  
    } 
    return (true);  
}
#endif

static int nuTolowerAndSlash(int c)
{
    if (c >= 'A' && c <= 'Z')
    {
        return c - 'A' + 'a';
    }
    else if ('\\' == c)
    {
        c = '/';
    }

    return c;
}

///Open APIs Interface.////////////////////////////////////////////////////
// File API
nuFILE *nuFopen(const nuCHAR *sFileName, nuINT nMode)
{
	FILE *pFile = NULL;

    char sBuf[256];
    int i;
    
    for (i = 0; i < 255 && sFileName[i]; i++)
    {
        sBuf[i] = nuTolowerAndSlash(sFileName[i]);
    }
    sBuf[i] = '\0';
    sFileName = sBuf;
	
    switch(nMode)
	{
	case NURO_FS_RB:
		pFile = fopen(sFileName, "rb");
		break;

	case NURO_FS_WB:
		pFile = fopen(sFileName, "wb");
		break;

	case NURO_FS_OB:
		pFile = fopen(sFileName, "rb+");
		if (!pFile)
		{
			pFile = fopen(sFileName, "wb+");
		}
		break;

	case NURO_FS_OB2:
		pFile = fopen(sFileName, "wb+");
		break;

	case NURO_FS_AB:
		pFile = fopen(sFileName, "rb+");
		if (!pFile)
		{
			pFile = fopen(sFileName, "wb+");
		}
		else
		{
			nuFseek((nuFILE *)pFile, 0, NURO_SEEK_END);
		}
		break;

	default:
		pFile = fopen(sFileName, "wb+");
		break;
	}

    	if(!pFile)
	{
		__android_log_print(ANDROID_LOG_INFO, "navi", "File Open Fail %s", sFileName);
	}
   /*if (!pFile)
    {
        printf("file opened [%s] failed!\n", sFileName);
    }
   else
    {
        printf("file opened [%s] sucessfully!\n", sFileName);
    }
    */
	return (nuFILE*)pFile;
}

nuFILE *nuWfopen(const nuWCHAR *sFileName, nuINT nMode)
{
#ifdef NURO_OS_WINDOWS
	FILE *pFile = NULL;

	switch (nMode)
	{
		case NURO_FS_RB:
			pFile = _wfopen(sFileName, L"rb");
			break;
		
		case NURO_FS_WB:
			pFile = _wfopen(sFileName, L"wb");
			break;
		
		case NURO_FS_OB:
			pFile = _wfopen(sFileName, L"rb+");
			if (!pFile)
			{
				pFile = _wfopen(sFileName, L"wb+");
			}
			break;
		
		case NURO_FS_OB2:
			pFile = _wfopen(sFileName, L"wb+");
			break;
		
		case NURO_FS_AB:
			pFile = _wfopen(sFileName, L"rb+");
			if (!pFile)
			{
				pFile = _wfopen(sFileName, L"wb+");
			}
			else
			{
				nuFseek((nuFILE *)pFile, 0, NURO_SEEK_END);
			}
			break;
		
		default:
			pFile = _wfopen(sFileName, L"wb+");
			break;
	}

	return (nuFILE *)pFile;
#endif

#ifdef NURO_OS_LINUX
    return NULL;
#endif
}

nuINT	nuFseek(nuFILE *pFile, nuLONG nDis, nuINT nType)
{
	return fseek((FILE *)pFile, nDis, nType);
}

nuSIZE	nuFwrite(const nuVOID *pBuff, nuSIZE size, nuSIZE count, nuFILE *pFile)
{
	return fwrite(pBuff, size, count, (FILE *)pFile);
}

nuSIZE	nuFread(nuVOID *pBuff, nuSIZE size, nuSIZE count, nuFILE *pFile)
{
	return fread(pBuff, size, count, (FILE *)pFile);
}

nuINT	nuFclose(nuFILE *pFile)
{
	return fclose((FILE*)pFile);
}

nuSIZE	nuFtell(nuFILE *pFile)
{
	return ftell((FILE *)pFile);
}

nuSIZE	nuFgetlen(nuFILE *pFile)
{
	nuSIZE i = nuFtell(pFile);
	nuSIZE n = 0;
	nuFseek(pFile, 0L, NURO_SEEK_END);
	n = nuFtell(pFile);
	nuFseek(pFile, i, NURO_SEEK_SET);
	return n;
}

// DLL load

#ifndef WINCE
nuHMODULE	nuLoadLibraryA(const nuCHAR *sFile)
{
#ifdef NURO_OS_WINDOWS
	return (nuHMODULE)LoadLibraryA(sFile);
#endif
#ifdef NURO_OS_LINUX
    char sBuf[256];
    int i;
    
    for (i = 0; i < 255 && sFile[i]; i++)
    {
        sBuf[i] = nuTolowerAndSlash(sFile[i]);
    }
    sBuf[i] = '\0';
    sFile = sBuf;
    nuHMODULE fn = (nuHMODULE)dlopen(sFile, RTLD_LAZY);
    
    if (!fn)
    {
        printf("Loading [%s] failed!\n", sFile);
        printf("%s\n", dlerror());
    }
    else
    {
        printf("Loading [%s] sucessfully!\n", sFile);
    }
    
    return fn;
#endif
}
#endif

nuHMODULE	nuLoadLibraryW(const nuWCHAR *wcsFile)
{
#ifdef NURO_OS_WINDOWS
	return (nuHMODULE)LoadLibraryW(wcsFile);
#endif
#ifdef NURO_OS_LINUX
    return NULL;
#endif
}

nuBOOL		nuFreeLibrary(nuHMODULE hInst)
{
#ifdef NURO_OS_WINDOWS
	return (nuBOOL)FreeLibrary((HMODULE)hInst);
#endif
#ifdef NURO_OS_LINUX
    nuINT flag = dlclose(hInst);
    //0 on success,nonzero on error
    if (0 == flag)
    {
        return nuTRUE;
    }
    else
    {
        return nuFALSE;
    } 
#endif
}

nuPVOID		nuGetProcAddress(nuHMODULE hModule, const nuCHAR *sFucName)
{
#ifdef NURO_OS_WINDOWS
#ifdef WINCE
	return GetProcAddressA(hModule, sFucName);
#else
	return GetProcAddress(hModule, sFucName);
#endif
#endif
#ifdef NURO_OS_LINUX
    return dlsym(hModule, sFucName); 
#endif
}


// Thread API
nuHANDLE nuCreateThread (PNURO_SECURITY_ATTRIBUTES lpsa, 
						 nuDWORD cbStack, 
						 LPNURO_THREAD_START_ROUTINE lpStartAddr,
						 nuPVOID lpvThreadParam,
						 nuDWORD fdwCreate,
						 nuDWORD *lpIDThread)
{
#ifdef NURO_OS_WINDOWS
	return (nuHANDLE)_beginthreadex((LPSECURITY_ATTRIBUTES)lpsa, cbStack, (unsigned ( __stdcall *)(void *))lpStartAddr, lpvThreadParam, fdwCreate, (unsigned *)lpIDThread);
#endif
#ifdef NURO_OS_LINUX
    pthread_t tid;
    int rc = pthread_create(
            &tid, NULL, 
            (void *(*)(void *))lpStartAddr, 
            lpvThreadParam);
    
    if (0 != rc)
    {
        return (nuHANDLE)-1;
    }
    
    return (nuHANDLE)tid;
#endif
}

nuBOOL nuDelThread(nuHANDLE hHandle)
{
#ifdef NURO_OS_WINDOWS
	return CloseHandle((HANDLE)hHandle);
#endif
#ifdef NURO_OS_LINUX
    int rc = pthread_detach((pthread_t)hHandle);

    if (0 != rc)
    {
        return nuFALSE;
    }
    
    return nuTRUE;
#endif
}

nuDWORD nuThreadWaitReturn(nuHANDLE hHandle)
{
#ifdef NURO_OS_WINDOWS
	DWORD ret;

	WaitForSingleObject((HANDLE)hHandle, INFINITE);
	GetExitCodeThread((HANDLE)hHandle, &ret);
	CloseHandle((HANDLE)hHandle);
	
	return ret;
#endif
#ifdef NURO_OS_LINUX
    nuDWORD nRet;

    if (0 != pthread_join((pthread_t)hHandle, (void **)&nRet))
    {
        return -1;
    }
    
    return nRet;
#endif
}

nuBOOL nuSetThreadPriority(nuHANDLE hHandle, nuINT nPriority)
{
#ifdef NURO_OS_WINDOWS
	return SetThreadPriority(hHandle, nPriority);
#endif
#ifdef NURO_OS_LINUX
    return nuFALSE;
#endif
}

#ifndef WINCE
//Process and data path
nuDWORD nuGetModulePathA(nuHMODULE hModule, nuCHAR  *lpFilename, nuDWORD nSize)
{
#ifdef NURO_OS_WINDOWS
	nuINT i;

	if( lpFilename == NULL)
	{
		return 0;
	}

	GetModuleFileNameA(hModule, lpFilename, nSize);

	for(i = (nuINT)nuStrlen(lpFilename) - 1; i >= 0; --i)
	{
		if('\\' == lpFilename[i])
		{
			lpFilename[i+1] = 0;
			break;
		}
	}

	return nuStrlen(lpFilename);
#endif
#ifdef ANDROID
    nuStrcpy(lpFilename, g_path);
    return nuStrlen(lpFilename);
#endif
}
#endif

nuDWORD nuGetModulePathW(nuHMODULE hModule, nuWCHAR *lpFilename, nuDWORD nSize)
{
#ifdef NURO_OS_WINDOWS
	nuINT i;

	if( lpFilename == NULL)
	{
		return 0;
	}
	
	GetModuleFileNameW(hModule, lpFilename, nSize);
	
	for(i = (nuINT)nuWcslen(lpFilename) - 1; i >= 0; --i)
	{
		if('\\' == lpFilename[i])
		{
			lpFilename[i+1] = 0;
			break;
		}
	}
	
	return nuWcslen(lpFilename);
#endif
#ifdef NURO_OS_LINUX
    return 0;
#endif
}

#ifndef WINCE
nuDWORD nuGetDataPathA(nuHINSTANCE hModule, nuCHAR  *lpFilename, nuDWORD nSize)
{
	return nuGetModulePathA(hModule, lpFilename, nSize);
}
#endif

nuDWORD nuGetDataPathW(nuHINSTANCE hModule, nuWCHAR *lpFilename, nuDWORD nSize)
{
#ifdef NURO_OS_WINDOWS
	return nuGetModulePathW(hModule, lpFilename, nSize);
#endif
#ifdef NURO_OS_LINUX
    return 0;
#endif
}

nuINT nuMbsToWcs(nuCHAR *pSrc, nuWCHAR *pDes, nuINT nMaxWords)
{
#ifdef NURO_OS_WINDOWS
	nuINT dwMinSize = MultiByteToWideChar(CP_ACP, 0, pSrc, -1, NULL, 0);
	
	if (!nMaxWords)
	{
		return dwMinSize;
	}
	
	if(nMaxWords < dwMinSize)
	{
		return 0;
	}
	
	return MultiByteToWideChar(CP_ACP, 0, pSrc, -1, pDes, dwMinSize);  
#endif

#ifdef NURO_OS_LINUX
    return 0;
#endif
}

nuINT nuWcsToMbs(nuWCHAR *pSrc, nuCHAR *pDes, nuINT nMaxWords)
{
#ifdef NURO_OS_WINDOWS
	nuINT dwMinSize = WideCharToMultiByte(CP_OEMCP, 0, pSrc, -1, NULL, 0, NULL, nuFALSE);
	
	if (!nMaxWords)
	{
		return dwMinSize;
	}
	
	if(nMaxWords < dwMinSize)
	{
		return 0;
	}

	return WideCharToMultiByte(CP_OEMCP , 0,pSrc ,-1 ,pDes ,dwMinSize, NULL, nuFALSE);
#endif
#ifdef NURO_OS_LINUX
    return 0;
#endif
}

nuSIZE	nuFreadLine(nuWCHAR *pBuff,  nuSIZE nMaxLen, nuFILE *pFile)
{
	nuUINT i = 0;
	nuWCHAR *p = pBuff;

	for(i = 0; i < nMaxLen - 1;)
	{
		if(nuFread((nuVOID*)p, sizeof(nuWCHAR), 1, pFile)==1)
		{
			if( i==0 && (*p==0x0A || *p==0x0D) )//ignore CR && LF
			{
				continue;
			}
			if( *p==0x0D || *p==0x0A)//end of one line
			{
				*p = 0x00;
				return (nuUINT)(p - pBuff);
			}
			else
			{
				p++;
				i++;
			}
		}
		else//end of file
		{
			*p = 0x00;
			return (nuUINT)(p - pBuff);
		}
	}

	//line too long
	*p = 0x00;
	return (nuUINT)(p - pBuff); 
}

nuSIZE	nuFreadLineA(nuCHAR *pBuff,  nuSIZE nMaxLen, nuFILE *pFile)
{
	nuUINT i = 0;
	nuCHAR *p = pBuff;

	for( i = 0; i < nMaxLen - 1; )
	{
		if( nuFread((nuVOID*)p, sizeof(nuCHAR), 1, pFile) == 1 )
		{
			if( i==0 && (*p==0x0A || *p==0x0D) )//ignore CR && LF
			{
				continue;
			}
			if( *p==0x0D || *p==0x0A)//end of one line
			{
				*p = 0x00;
				return (nuUINT)(p - pBuff);
			}
			else
			{
				i++;
				p++;
			}
		}
		else//end of file
		{
			*p = 0x00;
			return (nuUINT)(p - pBuff);
		}
	}
	//line too long
	*p = 0x00;
	return (nuUINT)(p - pBuff); 
}

nuVOID nuSleep(nuDWORD nMiniseconds)
{
#ifdef NURO_OS_WINDOWS
	Sleep(nMiniseconds);
#endif
#ifdef NURO_OS_LINUX
    if ((nMiniseconds / 1000) > 0)
    {
        sleep(nMiniseconds / 1000);
    }
    usleep((nMiniseconds % 1000) * 1000);
#endif
}

nuDWORD nuGetTickCount()
{
#ifdef NURO_OS_WINDOWS
	return GetTickCount();
#endif
#ifdef NURO_OS_LINUX
#ifdef ANDROID
#define UPTIME_FILE "/proc/uptime"
    static long time_stamp;
    struct timeval current_time;
    gettimeofday(&current_time, NULL);
    if (time_stamp == 0) {
        int fd = -1;
        if ((fd = open(UPTIME_FILE, O_RDONLY)) == -1) {
            perror("open error");
            exit(EXIT_FAILURE);
        }
        char buf[128];
        if (read(fd, buf, 128) <= 0) {
            perror("read error");
            exit(EXIT_FAILURE);
        }
        close(fd);
        char *p = buf;
        while (*p++ != ' ')
            ;
        *--p = '\0';
        long tmp = (long)(1000 * atof(buf));
        time_stamp = current_time.tv_sec * 1000 + current_time.tv_usec / 1000 - tmp;
        return (unsigned int)tmp;
    } else {
        return (unsigned int)(current_time.tv_sec * 1000 +
                current_time.tv_usec / 1000 - time_stamp);
    }
#undef UPTIME_FILE
    return -1;
#else
    struct timespec ts; 
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
#endif
#endif
}

#define       LINELEN	256
nuBOOL nuReadConfigValue(const nuCHAR *pStr, nuLONG *pValue)
{
	nuBOOL res;
	if( pStr == NULL || pValue == NULL )
	{
		return nuFALSE;
	}

	{
		nuCHAR strLine[LINELEN] = {0};
		nuINT nStr2Len, nStrLen;
		nuINT nSp, nLp, nIdx;
		nuFILE *pFile;
#ifndef WINCE
		nuCHAR filename[NURO_MAX_PATH];
		nuGetModulePathA(NULL, filename, NURO_MAX_PATH);
		nuStrcat(filename, "setting\\config.ini");
		pFile = nuFopen(filename, NURO_FS_RB);
#else
		nuWCHAR filename[NURO_MAX_PATH];
		nuGetModulePathW(NULL, filename, NURO_MAX_PATH);
		nuWcscat(filename, L"setting\\config.ini");
		pFile = nuWfopen(filename, NURO_FS_RB);
#endif

		nStr2Len = 0;
		nStrLen = nuStrlen(pStr);
		res = nuFALSE;
		
		if (NULL == pFile)
		{
			return nuFALSE;
		}

		nuMemset(strLine, 0, sizeof(strLine));

		while( nuFreadLineA(strLine, LINELEN, pFile) != 0 )
		{
			nIdx = nSp = nLp = 0;
			strLine[LINELEN-1] = '\0';
			nStr2Len = nuStrlen(strLine)+1;
			while( strLine[nIdx] != '\0' )
			{
				if( strLine[nIdx] == '[' )
				{
					break;
				}
				nIdx ++;
			}
			nIdx ++;
			while( nIdx < nStr2Len && strLine[nIdx] == ' ' )
			{
				nIdx++;
			}
			if( nIdx + nStrLen < nStr2Len )
			{
				nuINT idx = 0;
				while( idx < nStrLen )
				{
					if( strLine[nIdx] != pStr[idx] )
					{
						break;
					}
					idx ++;
					nIdx ++;
				}
				if( idx == nStrLen )
				{
					while( nIdx < nStr2Len && strLine[nIdx] == ' ' )
					{
						nIdx++;
					}
					if( nIdx < nStr2Len && strLine[nIdx] == ']' )
					{
						nIdx ++;
						while( nIdx < nStr2Len && strLine[nIdx] != '=' )
						{
							nIdx ++;
						}
						if( nIdx < nStr2Len )
						{
							nIdx ++;
							while( nIdx < nStr2Len && strLine[nIdx] == ' ' )
							{
								nIdx++;
							}
							if( nIdx < nStr2Len )
							{
								char *pEndStr;
								*pValue = nuStrtol(&strLine[nIdx], &pEndStr, 10);
								res = nuTRUE;
								break;
							}
						}
					}
				}
			}
			nuMemset(strLine, 0, sizeof(char) * LINELEN);
		}
		nuFclose(pFile);
	}
	

	return res;
}

nuSIZE  nuFreadLineIndexW(nuINT nLineIdx, nuWCHAR *pBuff, nuSIZE nWsNum, nuFILE *pFile)
{
	nuSIZE nNum = 0;
	if (nLineIdx < 0 || pFile == NULL || pBuff == NULL)
	{
		return 0;
	}

	{
		nuWCHAR wsTmp[256];
		while (nuFreadLine(wsTmp, 256, pFile) != 0)
		{
			if( nLineIdx == 0 )
			{
				nNum = nuWcslen(wsTmp);
				nNum = _MIN(nNum, nWsNum-1);
				nuWcsncpy(pBuff, wsTmp, nNum);
				pBuff[nNum] = 0;
				break;
			}
			else
			{
				--nLineIdx;
			}
		}
	}

	return nNum;
}

nuVOID nuGetLocalTime(NUROTIME* pnuTime)
{
	if(pnuTime == NULL)
	{
		return;
	}
	nuMemset(pnuTime, 0 , sizeof(NUROTIME));

#ifdef NURO_OS_WINDOWS
	GetLocalTime((LPSYSTEMTIME)pnuTime);
#endif

#ifdef NURO_OS_UCOS
	GIS_Time_Attr_t time;
	g_pnuOs->pfMcatchGpsGetTime(&time);
	pnuTime->nDay		= time.tm_mday;
	pnuTime->nMonth		= time.tm_mon;
	pnuTime->nYear		= time.tm_year;
	pnuTime->nSecond	= time.tm_sec;
	pnuTime->nMinute	= time.tm_min;
	pnuTime->nHour		= time.tm_hour;
#endif

#ifdef NURO_OS_ECOS	
	if (nuFALSE == g_bIsSetLocalTime)
	{
		nuMemset(&g_LocalNUROTIME, NULL, sizeof(g_LocalNUROTIME));
	}

	pnuTime->nDay		= g_LocalNUROTIME.nDay;
	pnuTime->nMonth		= g_LocalNUROTIME.nMonth;
	pnuTime->nYear		= g_LocalNUROTIME.nYear;
	pnuTime->nSecond	= g_LocalNUROTIME.nSecond;
	pnuTime->nMinute	= g_LocalNUROTIME.nMinute;
	pnuTime->nHour		= g_LocalNUROTIME.nHour;
#endif
#ifdef NURO_OS_HV_UCOS
	nuWORD nYear;
	nuBYTE nMonth, nDay, nMinute, nHour,nSecond;
	HV_GetDate(&nYear, &nMonth, &nDay);
	HV_GetTime(&nHour, &nMinute, &nSecond);
	pnuTime->nYear	= nYear;
	pnuTime->nMonth	= nMonth;
	pnuTime->nDay	= nDay;
	pnuTime->nHour	= nHour;
	pnuTime->nMinute	= nMinute;
	pnuTime->nSecond	= nSecond;
#endif
#ifdef NURO_OS_LINUX
	time_t curtime;
	struct tm *plocaltime;
	time(&curtime);
	plocaltime = localtime(&curtime);
	pnuTime->nDay		= plocaltime->tm_mday;
	pnuTime->nMonth		= plocaltime->tm_mon + 1;
	pnuTime->nYear		= plocaltime->tm_year + 1900;
	pnuTime->nSecond	= plocaltime->tm_sec;
	pnuTime->nMinute	= plocaltime->tm_min;
	pnuTime->nHour		= plocaltime->tm_hour;
#endif
}

nuBOOL nuSetLocalTime(NUROTIME* pnuTime)
{
	if(pnuTime == NULL)
	{
		return nuFALSE;
	}
#ifdef NURO_OS_WINDOWS
	if( SetLocalTime((LPSYSTEMTIME)pnuTime) )
	{
		return nuTRUE;
	}
	else
	{
		return nuFALSE;
	}
#endif
#ifdef NURO_OS_UCOS
	GIS_Time_Attr_t time;
	time.tm_mday	= pnuTime->nDay;
	time.tm_mon		= pnuTime->nMonth ;
	time.tm_year	= pnuTime->nYear ;
	time.tm_sec		= pnuTime->nSecond ;
	time.tm_min		= pnuTime->nMinute ;
	time.tm_hour	= pnuTime->nHour ;

	if( !g_pnuOs->pfMcatchGpsSetTime(&time))
	{
		return nuTRUE;
	}

	return nuFALSE;
#endif
#ifdef NURO_OS_ECOS
	g_LocalNUROTIME.nDay    = pnuTime->nDay;
	g_LocalNUROTIME.nMonth  = pnuTime->nMonth;
	g_LocalNUROTIME.nYear   = pnuTime->nYear;
	g_LocalNUROTIME.nSecond = pnuTime->nSecond;
	g_LocalNUROTIME.nMinute = pnuTime->nMinute;
	g_LocalNUROTIME.nHour   = pnuTime->nHour;

	g_bIsSetLocalTime = nuTRUE;
	return nuTRUE;
#endif
#ifdef NURO_OS_HV_UCOS
	HV_SetDate( pnuTime->nYear, (nuBYTE)pnuTime->nMonth, (nuBYTE)pnuTime->nDay );
	HV_SetTime( (nuBYTE)pnuTime->nHour, (nuBYTE)pnuTime->nMinute, (nuBYTE)pnuTime->nSecond );
#endif
#ifdef NURO_OS_LINUX
	struct tm localtm;
	nuMemset(&localtm, 0, sizeof(struct tm));
	localtm.tm_year = pnuTime->nYear - 1900;
	localtm.tm_mon 	= pnuTime->nMonth -1;
	localtm.tm_mday = pnuTime->nDay;
	localtm.tm_hour = pnuTime->nHour;
	localtm.tm_min 	= pnuTime->nMinute;
	localtm.tm_sec 	= pnuTime->nSecond;
	time_t curtime;
	curtime = mktime(&localtm);
#ifdef ANDROID
	return nuTRUE;
#else
	if (!stime(&curtime))
		return nuTRUE;
	else
		return nuFALSE;
#endif
#endif
	return nuFALSE;
}

nuVOID	nuSetVolume(nuUINT iVol)
{
	//nuLONG ii = 0xFFFF / 10 * iVol;
	//iVol = iVol;
	//iVol /= _MIN(iVol, 0xFFFF);
	// = _MAX(iVol, 0);
/*#ifdef NURO_OS_WINDOWS
	// iVol *= iVol;
	// iVol *= iVol;
	waveOutSetVolume(0, (ii<<12) | (ii<<28)); 
#endif
*/
	nuUINT iVolMax = 0;
	if( !nuReadConfigValue("VOLMAX", &iVolMax) )
	{
		iVolMax = 0xFFFF;
	}
#ifdef VALUE_EMGRT
	if (0 != iVol)
	{
		iVol = iVolMax/16*iVol;
	}
#else
	if (0 != iVol)
	{
		iVol = (iVolMax/2) + (iVolMax*iVol/20);
	}
#endif

	
	//iVol = iVolMax/10*iVol;
	//iVol = iVolMax/10*iVol;
#ifdef NURO_OS_WINDOWS
	//	iVol *= iVol;
	//	iVol *= iVol;
	//waveOutSetVolume(0, (iVol<<12) | (iVol<<28));	
	waveOutSetVolume(0, (iVol<<16) | (iVol));
#endif

#ifdef NURO_OS_HV_UCOS
	iVol = (iVol*30)/15;
	HV_SetVol(iVol);
#endif

#ifdef NURO_OS_UCOS
	iVol*=2;
	iVol = (iVol*11)/15;
	g_pnuOs->pfMcatchGPSVolumeSet((UINT16)iVol);
#endif

#ifdef NURO_OS_ECOS
	MCI_SET_AUDIO_PARAM setAudioParam;

	setAudioParam.Item = MCI_SET_AUDIO_VOLUME;
	setAudioParam.Data = (nuULONG) iVol << 28; 

	if (!g_pnuOsMCIDevice)
	{
		DebugOut("g_pnuOsMCIDevice error!\n");
		return;
	}

	mciSendCommand(g_pnuOsMCIDevice, 
		MCI_SET_AUDIO, 
		MCI_FLAG_SET_AUDIO_ITEM | MCI_FLAG_SET_AUDIO_L | MCI_FLAG_SET_AUDIO_R,
		(nuULONG)  &setAudioParam);

#endif
}

nuUINT	nuGetVolume()
{
#ifdef NURO_OS_WINDOWS
	nuDWORD iVol = 0;
	waveOutGetVolume(0, &iVol);
	return _MAX(_LOWORD(iVol), _HIWORD(iVol))>>12;
#endif
#ifdef NURO_OS_UCOS
	//
	return 0;
#endif
#ifdef  NURO_OS_HV_UCOS
	return HV_GetVol();
#endif
	return 0;
}

#ifndef WINCE
nuBOOL nuCopyFileA(const nuCHAR *lpExistingFileName, const nuCHAR *lpNewFileName, nuBOOL bFailIfExists)
{
#ifdef NURO_OS_WINDOWS
	return CopyFileA(lpExistingFileName, lpNewFileName, bFailIfExists);
#endif
#ifdef NURO_OS_LINUX
    return nuFALSE;
#endif
}
#endif

nuBOOL nuCopyFileW(const nuWCHAR *lpExistingFileName, const nuWCHAR *lpNewFileName, nuBOOL bFailIfExists)
{
#ifdef NURO_OS_WINDOWS
	return CopyFileW(lpExistingFileName, lpNewFileName, bFailIfExists);
#endif
#ifdef NURO_OS_LINUX
    return nuFALSE;
#endif
}

#ifndef WINCE
nuBOOL nuPlaySoundA(nuCHAR *pszSound, nuBYTE *soundbuff, nuWORD nSyncOrASync)
{
#ifdef NURO_OS_WINDOWS
	return PlaySoundA(pszSound, (HMODULE)soundbuff, nSyncOrASync);
#endif
#ifdef NURO_OS_LINUX
    printf("%s\n", pszSound);
    return nuFALSE;
#endif
}
#endif

nuBOOL nuPlaySoundW(nuWCHAR *pszSound, nuBYTE *soundbuff, nuWORD nSyncOrASync)
{
#ifdef NURO_OS_WINDOWS
	return PlaySoundW(pszSound, (HMODULE)soundbuff, nSyncOrASync);
#endif
#ifdef NURO_OS_LINUX
    return nuFALSE;
#endif
}


#ifdef _USE_DATA_MEMORY_OF_FILE_MAPPING
static HANDLE gc_hFileMapping = NULL;
#endif

nuBOOL nuGetMemory(nuBYTE **ppStaticMemAddr, nuDWORD dwSMsize, nuBYTE **ppDataMemAddr, nuDWORD dwDMsize)
{
#ifdef nuGetMemory_done
#   undef nuGetMemory_done
#endif

#ifdef NURO_OS_WINDOWS
#define nuGetMemory_done
	nuPVOID  lpAddress;
	(*ppStaticMemAddr) = (nuBYTE*)VirtualAlloc(NULL, (DWORD)dwSMsize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
	if( *ppStaticMemAddr == NULL )
	{
		return nuFALSE;
	}
	nuMemset(*ppStaticMemAddr, 0, dwSMsize);

#ifndef _USE_DATA_MEMORY_OF_FILE_MAPPING
	(*ppDataMemAddr) = (nuBYTE*)VirtualAlloc(NULL, (DWORD)dwDMsize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
	if( *ppDataMemAddr == NULL )
	{
		VirtualFree((LPVOID)( *ppStaticMemAddr), 0, MEM_RELEASE);
		return nuFALSE;
	}
	
#else
	gc_hFileMapping = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, dwDMsize, NULL);
	if(gc_hFileMapping == NULL)
	{
		return nuFALSE;
	}

	
	lpAddress = MapViewOfFile(gc_hFileMapping, FILE_MAP_WRITE|FILE_MAP_READ, 0, 0, dwDMsize);

	if( lpAddress == NULL )
	{
		return nuFALSE;
	}
	*ppDataMemAddr = (nuBYTE*)lpAddress;

#endif
	return nuTRUE;
#endif

#ifndef nuGetMemory_done
	(*ppStaticMemAddr)  = (nuBYTE*)nuMalloc(dwSMsize);
	if( *ppStaticMemAddr == NULL )
	{
		return nuFALSE;
	}
	(*ppDataMemAddr) = (nuBYTE*)nuMalloc(dwDMsize);
	if( *ppDataMemAddr == NULL )
	{
		nuFree( *ppStaticMemAddr);
		return nuFALSE;
	}
	return nuTRUE;
#endif
}

nuVOID nuRelMemory(nuBYTE *pStaticMemAddr, nuBYTE *pDataMemAddr)
{
#ifdef nuRelMemory_done
#   undef nuRelMemory_done
#endif
#ifdef NURO_OS_WINDOWS
#   define nuRelMemory_done

#ifndef _USE_DATA_MEMORY_OF_FILE_MAPPING
	if ( pDataMemAddr )     VirtualFree((LPVOID)pDataMemAddr, 0, MEM_RELEASE);
#else
	if( pDataMemAddr != NULL )
	{
		UnmapViewOfFile(pDataMemAddr);
	}
	if( gc_hFileMapping != NULL )
	{
		CloseHandle(gc_hFileMapping);
		gc_hFileMapping = NULL;
	}
#endif


	if ( pStaticMemAddr )   VirtualFree((LPVOID)pStaticMemAddr, 0, MEM_RELEASE);
#endif
#ifndef nuRelMemory_done
	if ( pDataMemAddr )     nuFree(pDataMemAddr);
	if ( pStaticMemAddr )	nuFree(pStaticMemAddr);
#endif
}

#ifndef WINCE
nuHANDLE nuOpenComA(nuCHAR *gps_comport, nuDWORD nBaudRate)
{
//__android_log_print((ANDROID_LOG_INFO, "usb", "nuOpenComA");
#ifdef NURO_OS_WINDOWS
	nuHANDLE hHandle = CreateFileA(gps_comport, GENERIC_READ| GENERIC_WRITE , 0, NULL, 
		OPEN_EXISTING, FILE_ATTRIBUTE_SYSTEM, NULL);
	
	if (NURO_INVALID_HANDLE_VALUE == hHandle)
	{
		return NURO_INVALID_HANDLE_VALUE;
	}

	{
		DCB dcb;
		COMMTIMEOUTS cto; 
		
		if (nuFALSE == GetCommState(hHandle, &dcb))
		{
			CloseHandle(hHandle);
			return NURO_INVALID_HANDLE_VALUE;
		}

		dcb.DCBlength = sizeof (DCB);
		dcb.BaudRate = nBaudRate;
		dcb.ByteSize	= 8;
		dcb.Parity		= NOPARITY; 
		dcb.StopBits	= ONESTOPBIT;

		if (nuFALSE == SetCommState(hHandle, &dcb))
		{
			CloseHandle(hHandle);
			return NURO_INVALID_HANDLE_VALUE;
		}

		if(nuFALSE == GetCommTimeouts(hHandle, &cto))
		{
			return NURO_INVALID_HANDLE_VALUE;
		}
#ifdef DAIWOO
		cto.ReadIntervalTimeout			= 0;//MAXDWORD;
#else
		cto.ReadIntervalTimeout			= MAXDWORD;
#endif
		cto.ReadTotalTimeoutMultiplier	= 0;
		cto.ReadTotalTimeoutConstant	= 0;

		if(nuFALSE == SetCommTimeouts (hHandle, &cto))
		{
			return NURO_INVALID_HANDLE_VALUE;
		}
#ifndef DAIWOO
		if(nuFALSE == SetCommMask(hHandle, EV_RXCHAR))
		{
			return NURO_INVALID_HANDLE_VALUE;
		}	
#endif
	}

	return hHandle;
#endif
#ifdef NURO_OS_LINUX
	m_fd = open( UARTDEV, O_RDONLY);//| O_NOCTTY | O_NDELAY   
	if (-1 == m_fd)   
	{           
		//__android_log_print((ANDROID_LOG_INFO, "usb", "open fail");
		return NULL;      
	} 
	set_speed(m_fd, 38400);
    	set_Parity(m_fd, 8, 1, 'N');

	//__android_log_print((ANDROID_LOG_INFO, "usb", "A open ok");
    	return NULL;
#endif
}
#endif
nuHANDLE nuOpenComW(nuWCHAR *gps_comport, nuDWORD nBaudRate)
{
//__android_log_print((ANDROID_LOG_INFO, "usb", "nuOpenComW");
#ifdef NURO_OS_WINDOWS
	nuHANDLE hHandle = CreateFileW(gps_comport, GENERIC_READ/*| GENERIC_WRITE*/ , 0, NULL, 
		OPEN_EXISTING, FILE_ATTRIBUTE_SYSTEM, NULL);

	if (NURO_INVALID_HANDLE_VALUE == hHandle)
	{
		return NURO_INVALID_HANDLE_VALUE;
	}

	{
		DCB dcb;
		COMMTIMEOUTS cto; 

		if (nuFALSE == GetCommState(hHandle, &dcb))
		{
			CloseHandle(hHandle);
			return NURO_INVALID_HANDLE_VALUE;
		}

		dcb.DCBlength = sizeof (DCB);
		dcb.BaudRate = nBaudRate;
		dcb.ByteSize	= 8;
		dcb.Parity		= NOPARITY; 
		dcb.StopBits	= ONESTOPBIT;

		if (nuFALSE == SetCommState(hHandle, &dcb))
		{
			CloseHandle(hHandle);
			return NURO_INVALID_HANDLE_VALUE;
		}

		if(nuFALSE == GetCommTimeouts(hHandle, &cto))
		{
			return NURO_INVALID_HANDLE_VALUE;
		}
#ifdef DAIWOO
		cto.ReadIntervalTimeout			= 0;//MAXDWORD;
#else
        cto.ReadIntervalTimeout			= MAXDWORD;
#endif
		cto.ReadTotalTimeoutMultiplier	= 0;
		cto.ReadTotalTimeoutConstant	= 0;

		if(nuFALSE == SetCommTimeouts (hHandle, &cto))
		{
			return NURO_INVALID_HANDLE_VALUE;
		}
#ifndef DAIWOO
		if(nuFALSE == SetCommMask(hHandle, EV_RXCHAR))
		{
			return NURO_INVALID_HANDLE_VALUE;
		}
#endif	
	}

	return hHandle;
#endif


#ifdef NURO_OS_LINUX
	m_fd = open( UARTDEV, O_RDONLY);//| O_NOCTTY | O_NDELAY   
	if (-1 == m_fd)   
	{           
		//__android_log_print((ANDROID_LOG_INFO, "usb", "open fail");
		return NULL;      
	} 
	set_speed(m_fd, 38400);
    	set_Parity(m_fd, 8, 1, 'N');

	//__android_log_print((ANDROID_LOG_INFO, "usb", "W open ok");
    	return NULL;
#endif

}

nuBOOL nuCloseCom(nuHANDLE hCom)
{
#ifdef NURO_OS_WINDOWS
	return CloseHandle(hCom);
#endif
#ifdef NURO_OS_LINUX
    close(m_fd);
    return nuFALSE;
   
#endif
}
nuBOOL nuOpenOtherCom()
{
	////__android_log_print((ANDROID_LOG_INFO, "TT", "nuOpenOtherCom fail");
	close(m_fd);
	m_fd = -1;
	if(m_fd==-1)
	{
		m_fd = open( reOpenUARTDEV, O_RDONLY);//| O_NOCTTY | O_NDELAY   
		if (-1 == m_fd)   
		{           
			//__android_log_print((ANDROID_LOG_INFO, "TTYUSB", "open fail");
			return nuFALSE;      
		} 
		set_speed(m_fd, _nRate);
    		set_Parity(m_fd, 8, 1, 'N');
	}
	//__android_log_print((ANDROID_LOG_INFO, "TTYUSB", "nuOpenOtherCom ok");
	return nuTRUE;
}
nuBOOL nuReadCom(nuHANDLE hCom, nuPVOID buffer, nuDWORD nNumberOfBytesToRead,
				 nuPDWORD lpNumberOfBytesRead, nuPVOID pOverlapped)
{
#ifdef NURO_OS_WINDOWS
	return ReadFile(hCom, buffer, nNumberOfBytesToRead,
		lpNumberOfBytesRead, pOverlapped);
#endif
#ifdef NURO_OS_LINUX
	if(m_fd==-1)
	{
		m_fd = open( UARTDEV, O_RDONLY);//| O_NOCTTY | O_NDELAY   
		//m_fd = open( reOpenUARTDEV, O_RDONLY);//| O_NOCTTY | O_NDELAY  		
		if (-1 == m_fd)   
		{           
			//__android_log_print((ANDROID_LOG_INFO, "usb", "open fail");
			return nuFALSE;      
		} 
		set_speed(m_fd, _nRate);
    		set_Parity(m_fd, 8, 1, 'N');
	}
	//__android_log_print((ANDROID_LOG_INFO, "usb", "open read !!!!");
	nuDWORD n =   read(m_fd, buffer,nNumberOfBytesToRead);
	//__android_log_print((ANDROID_LOG_INFO, "usb", "%s",buffer);
	*lpNumberOfBytesRead= n;
	////__android_log_print((ANDROID_LOG_INFO, "usb", "lpNumberOfBytesRead:%d",n);
	return nuTRUE;
////__android_log_print((ANDROID_LOG_INFO, "usb", "open read !!!$$$$ %s",buffer[0]);
    //return nuFALSE;
#endif
}

nuBOOL nuWriteCom(nuHANDLE hCom, nuPVOID buffer, nuDWORD nNumberOfBytesToWrite,
				 nuPDWORD lpNumberOfBytesWritten, nuPVOID pOverlapped)
{
#ifdef NURO_OS_WINDOWS
	return WriteFile(hCom, buffer, nNumberOfBytesToWrite,
		lpNumberOfBytesWritten, pOverlapped);
#endif
#ifdef NURO_OS_LINUX
    return nuFALSE;
#endif
}
nuVOID nuSetModulePath(nuCHAR *strPathName)
{
     __android_log_print(ANDROID_LOG_INFO, "navi", "Set Nuro Path %s", strPathName);
	nuStrcpy(g_path,strPathName);
}

nuVOID PostMessage_ZH_Msg( nuLONG Msg, nuLONG wParam, nuLONG lParam )
{
#ifdef NURO_OS_WINDOWS
	HWND zh_win= NULL;
	nuLONG lVenderType = 0;
	if(0 == nuReadConfigValue("VENDERTYPE", &lVenderType))
	{
		return ;
	}
	else if(lVenderType != _VENDER_DSXW)
	{
		return ;
	}
	zh_win= FindWindow(AP_Main_ClassName, AP_Main_WindowName); //???ʹ?????ͬʱ????;
	if(zh_win == NULL)
	{
		zh_win= FindWindow(NULL, AP_Main_WindowName); //????????????;
	}
	if(zh_win == NULL)
	{
		zh_win = FindWindow(AP_Main_ClassName,NULL); //????????;
	}

	if(zh_win)
	{
		if(!PostMessage(zh_win, (UINT)Msg,(WPARAM)wParam,(LPARAM)lParam))
		{
			PostMessage(HWND_BROADCAST,(UINT)Msg,(WPARAM)wParam,(LPARAM)lParam);
		}
		zh_win = NULL;
		return ;
	}
#endif
}
