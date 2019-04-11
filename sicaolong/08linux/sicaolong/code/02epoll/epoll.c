#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <sys/epoll.h>

int main(int argc, const char* argv[])
{
    if(argc < 2)
    {
        printf("eg: ./a.out port\n");
        exit(1);
    }
    struct sockaddr_in serv_addr;
    socklen_t serv_len=sizeof(serv_addr);
    int port=atoi(argv[1]);
    //创建套接字；
    int lfd=socket(AF_INET,SOCK_STREAM,0);
    //初始化服务器 sockaddr_in;
    memset(&serv_addr,0,serv_len);
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_addr.sin_port=htons(port);
    //绑定ip地址和端口
    bind(lfd,(struct sockaddr*)&serv_addr,serv_len);
    //设置同时监听的最大个数；
    listen(lfd,128);
    printf("start accept.......\n");

    struct sockaddr_in client_addr;
    socklen_t client_len;

    //创建epfd
    int epfd=epoll_create(2000);
    //初始化epoll树；
    struct epoll_event ev;
    ev.events=EPOLLIN;
    ev.data.fd=lfd;
    epoll_ctl(epfd,EPOLL_CTL_ADD,lfd,&ev);

    struct epoll_event all[2000];
    while(1){
    	//使用epoll通知内核fd 文件io检测；
    	int ret=epoll_wait(epfd,all,sizeof(all)/sizeof(all[0]),-1);
    	//遍历all数组中的前ret个元素；
    	for (int i = 0; i < ret; ++i)
    	{
    		int fd=all[i].data.fd;
    		//判断是否有新的链接；
    		if (fd==lfd)
    		{
    			//接受链接请求；
    			int cfd=accept(lfd,(struct sockaddr*)&client_addr,&client_len);
    			if(cfd==-1){
    				perror("accept error");
    				exit(1);
    			}
    			//将新的cfd挂接到树上；
    			struct epoll_event temp;
    			temp.events=EPOLLIN;
    			temp.data.fd=cfd;
    			epoll_ctl(epfd,EPOLL_CTL_ADD,cfd,&temp);

    			 // 打印客户端信息
                char ip[64] = {0};
                printf("New Client IP: %s, Port: %d\n",
                    inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, ip, sizeof(ip)),
                    ntohs(client_addr.sin_port));
    			//
    		}
    		//处理已经链接的客户端发送过来的数据；
    		else
    		{
    			if(!all[i].events & EPOLLIN)continue;
    			//读数据；
    			char buf[1024]={0};
    			int len=recv(fd,buf,sizeof(buf),0);
    			if (len==-1)
    			{
    				perror("recv error");
    				exit(1);
    			}
    			else if(len==0)
    			{
    				printf("client have closed..........\n");
    				ret=epoll_ctl(epfd,EPOLL_CTL_DEL,fd,NULL);
    				if (ret==-1)
    				{
    					perror("epoll_ctl_del error");
    					exit(1);
    				}
    				close(fd);
    			}
    			else{
    				printf("recv buf:%s\n",buf);
    				write(fd,buf,len);
    			}

    		}

    	}

    }
    close(lfd);
    return 0;
}
