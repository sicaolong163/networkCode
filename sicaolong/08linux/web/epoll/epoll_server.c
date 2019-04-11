#include <stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<sys/epoll.h>
#include<arpa/inet.h>
#include"epoll_server.h"
#include <sys/types.h>          /* See NOTES */
       #include <sys/socket.h>
#include<fcntl.h>
#define MAXSIZE 2000
void epoll_run(int port)
{
    //创建一个epoll树的根节点
    int epfd=epoll_create(MAXSIZE);
    if(epfd==-1)
    {
        perror("epoll create error");
        exit(1);
    }
    //要监听的结点；
    int lfd=init_listen(port,epfd);
    //委托内核进行检测
    struct epoll_event all[MAXSIZE];
    while(1)
    {
        int ret=epoll_wait(epfd,all,MAXSIZE,-1);
        if(ret==-1)
        {
            perror("epoll waite error");
            exit(1);
        }
        //遍历所发生变化的结点
        for(int i=0;i<ret;i++)
        {
            //判断是监听的还是通信的；
            //只处理读事件 其他的时间默认不处理
            struct epoll_event *pev=&all[i];
            if(!(pev->events & EPOLLIN))
            {
                //如果不是读事件
                continue;
            }
            if(pev->data.fd==lfd)
            {
                //接受链接请求；
                do_accept(lfd,epfd);
            }
            else
            {
                //读数据
                do_read(pev->data.fd,epfd);
            }

        }
    }
}
void do_read(int cfd,int epfd)
{
    //通信；将浏览器发过来的数据发送到buf中；
    char line[1024]={0};
	//读请求行 
	int len=get_line(cfd,line,sizeof(line));
    if(len==0)
    {
        printf("客户端断开了来按揭....\n");
        //关闭套接字 将cfd从epoll树上摘下来；
        disconnect(cfd,epfd);
    }
    else if(len==-1)
    {
        //关闭套接字 将cfd从epoll数上摘下来；
        perror("recv error");
        exit(1);
    }
    else
    {
        printf("请求行数据：%s\n",line);
        //buf中海油数据 继续读
        while(len>0)
        {
            char buf[1024]={0};
            len=get_line(cfd,buf,sizeof(buf));
            printf("========================请求头=========================\n");
            printf("--------------:%s",buf);
        }
    }
    //请求行 ：get 、xxxhttp/1.1
    //判断下是不是get请求；
    if(strncasecmp("get",line,3)==0)
    {
        //处理http的请求；
    }
}
//断开连接的函数
void disconnect(int cfd,int epfd)
{
    int ret=epoll_ctl(epfd,EPOLL_CTL_DEL,cfd,NULL);
    if(ret==-1)
    {
        perror("epoll del error");
        exit(1);
    }
    close(cfd);
}
//http请求处理
void http_request(const char*request)
{
    //拆分http请求行
    char method[12],path[1024],protocol[12];
    sscanf(request,"%[^] %[^] %[^]",method,path,protocol);
}
// 解析http请求消息的每一行内容
int get_line(int sock, char *buf, int size)
{
    int i = 0;
    char c = '\0';
    int n;
    while ((i < size - 1) && (c != '\n'))
    {
        n = recv(sock, &c, 1, 0);
        if (n > 0)
        {
            if (c == '\r')
            {
                n = recv(sock, &c, 1, MSG_PEEK);
                if ((n > 0) && (c == '\n'))
                {
                    recv(sock, &c, 1, 0);
                }
                else
                {
                    c = '\n';
                }
            }
            buf[i] = c;
            i++;
        }
        else
        {
            c = '\n';
        }
    }
    buf[i] = '\0';
    if(n==-1)
    {
        i=-1;
    }
    return i;
}
void do_accept(int lfd,int epfd)
{
    struct sockaddr_in client_addr;
    socklen_t client_len=sizeof(client_addr);

    int cfd=accept(lfd,(struct sockaddr*)&client_addr,&client_len);
    if(cfd==-1)
    {
        perror("accept errror");
        exit(1);
    }
    //打印客户端的信息；
    char ip[64]={0};
    printf("new client ip:%s,port:%d\n",
           inet_ntop(AF_INET,&client_addr.sin_addr,ip,sizeof(ip)),
           ntohs(client_addr.sin_port));
    //设置cfd为非阻塞
    int flag=fcntl(cfd,F_GETFL);
    flag|=O_NONBLOCK;
    fcntl(cfd,F_SETFL,flag);
    //将得到的新节点挂接到epoll上
    struct epoll_event ev;
    ev.data.fd=cfd;
    //边缘非阻塞；
    ev.events=EPOLLIN|EPOLLET;
    int ret=epoll_ctl(epfd,EPOLL_CTL_ADD,cfd,&ev);
    if(ret==-1)
    {
        perror("epoll ctl error");
        exit(1);
    }
}
//初始化用于监听的套接字 封装了一个监听的文件描述符；
int init_listen(int port ,int epfd)
{
    //创建监听的套接字
    int lfd=socket(AF_INET,SOCK_STREAM,0);
    if(lfd==-1)
    {
        perror("socket error");
        exit(1);
    }
    //绑定本地ip和端口
    struct sockaddr_in server_addr;
    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(port);
    server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    //端口复用
    int flags=1;
    setsockopt(lfd,SOL_SOCKET,SO_REUSEADDR,&flags,sizeof(flags));
    int ret=bind(lfd,(struct sockaddr*)&server_addr,sizeof(server_addr));
    if(ret==-1)
    {
        perror("bind errror");
        exit(1);
    }
    //设置监听
    ret=listen(lfd,64);
    if(ret==-1)
    {
        perror("listen error");
        exit(1);
    }
    //lfd添加到epoll树上
    struct epoll_event ev;
    ev.events=EPOLLIN;
    ev.data.fd=lfd;
   ret=epoll_ctl(epfd,EPOLL_CTL_ADD,lfd,&ev);
   if(ret==-1)
   {
       perror("epoll ctl error");
       exit(1);
   }
   return lfd;
}
