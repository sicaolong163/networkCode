#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <sys/select.h>
#define SERV_PORT 8989
int main()
{
	int lfd,cfd;
	struct sockaddr_in serv_addr,client_addr;
	int serv_len,client_len;
	//创建套接字；
	lfd=socket(AF_INET,SOCK_STREAM,0);
	//初始化服务器 sockaddr_in;
	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_addr.sin_port=htons(SERV_PORT);
	serv_len=sizeof(serv_addr);
	//bind ip port;
	bind (lfd,(struct sockaddr*)&serv_addr,serv_len);
// 设置同时监听的最大个数
    listen(lfd, 36);
    printf("Start accept ......\n");
	client_len=sizeof(client_addr);

	int maxfd=lfd;
	fd_set reads,temp;
	//init 
	FD_ZERO(&reads);
	FD_SET(lfd,&reads);
	

	while(1){
		//委托内核做检测；
		temp=reads;
		int ret=select(maxfd+1,&temp,NULL,NULL,NULL);
		if(ret==-1){
			perror("select error");
			exit(1);
		}
		//客户端发起新的链接；
		if(FD_ISSET(lfd,&temp)){
			//接受链接请求；
			// 接受连接请求
            client_len = sizeof(client_len);
            int cfd = accept(lfd, (struct sockaddr*)&client_addr, &client_len);
            char ip[64];
            printf("new client ip:%s,Port:%d\n",inet_ntop(AF_INET,&client_addr.sin_addr.s_addr,ip,sizeof(ip)),
                   ntohs(client_addr.sin_port));
            // 文件描
            FD_SET(cfd, &reads);
            // 更新最大文件描述符
            maxfd = maxfd < cfd ? cfd : maxfd;
		}
		// 遍历检测的文件描述符是否有读操作
        for(int i=lfd+1; i<=maxfd; ++i)
        {
            if(FD_ISSET(i, &temp))
            {
                // 读数据
                char buf[1024] = {0};
                int len = read(i, buf, sizeof(buf));
                if(len  == -1)
                {
                    perror("read error");
                    exit(1);
                }
                else if(len == 0)
                {
                    // 对方关闭了连接
                    printf("客户端已经断开了链接\n");
                    FD_CLR(i, &reads);
                    close(i);
                    if(maxfd == i)
                    {
                        maxfd--;
                    }
          //          FD_CLR(i,&reads);          
                }
                else
                {
                    printf("read buf = %s\n", buf);
                    for(int j=0; j<len; ++j)
                    {
                        buf[j] = toupper(buf[j]);
                    }
                    printf("--buf toupper: %s\n", buf);
                    write(i, buf, strlen(buf)+1);
                }
            }
        }
	}
	close(lfd);
    return 0;
}

