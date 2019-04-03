#include <stdio.h>
#include <stdlib.h>
#include <utils/Log.h>


#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/uio.h>

#include "Socket_connect.h"

#define MAX_BUF_SIZE  (255)
char* name = "/data/CarIOSocket-navi";

extern "C"  int ConnectSocket()
{
    int sockfd,
        ret;
    
    struct sockaddr_un servaddr;

    socklen_t clilen;

    char buf[MAX_BUF_SIZE];
    ALOGD("[Navi client]socket client start\n");
    ALOGD("[Navi client]open a socket .... \n");
    /* open a socket */
    if ((sockfd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0)
    {
       ALOGD("[Navi client]socket failed\n");
       goto fun_exit;
    }
   
    ALOGD("[Navi client]set the addr .... \n");
    /* set the addr */
    //unlink(name);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sun_family = AF_LOCAL;
    strcpy(servaddr.sun_path, name);  

    /* connect to server */
    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
    {
       ALOGD("[Navi client]connect failed\n");
       goto fun_exit;
    }

    fd_set rset;
    FD_ZERO(&rset);
    FD_SET(sockfd, &rset);

    int res = 0;

    while(1)
    {
       ALOGD("[Navi client]select .....\n");
       select(sockfd+1, &rset, NULL, NULL, NULL);
       if (FD_ISSET(sockfd, &rset))
       {
           ALOGD("[Navi client]recv .....\n");
           res = recv(sockfd, buf, MAX_BUF_SIZE, 0);
           if(res <= 0)
           {
               ALOGD("[Navi client]recv failed\n");
               goto fun_exit;
           }
           else
           {
               ALOGD("[Navi client] recv: %s     :%d\n", buf, res);
           }
           
       }
       
    }
         
fun_exit:
    if (sockfd>0) close(sockfd);
       return ret;      
}

