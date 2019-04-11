#include <stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/un.h>
int main()
{
    int lfd=socket(AF_LOCAL,SOCK_STREAM,0);
    if(lfd==-1)
    {
        perror("socket error");
        exit(1);
    }
    //如果套接字存在 则替换；
    unlink("server.sock");
    //绑定
    struct sockaddr_un serv;
    serv.sun_family=AF_LOCAL;
    strcpy(serv.sun_path,"server.sock");
    int ret=bind(lfd,(struct sockaddr*)&serv,sizeof(serv));
    if(ret==-1)
    {
        perror("bind error");
        exit(1);
    }
    //监听
    ret=listen(lfd,36);
    //等待接收链接
    struct  sockaddr_un client;
    socklen_t client_len=sizeof(client);
    int cfd=accept(lfd,(struct sockaddr*)&client,&client_len);
    if(cfd==-1)
    {
        perror("accept error");
        exit(1);
    }
    //通信
    while(1)
    {
        char buf[1024]={0};
        int recv_len=recv(cfd,buf,sizeof(buf),0);
        if(recv_len==-1)
        {
            perror("recv error");
            exit(1);
        }
        else if(recv_len==0)
        {
            printf("客户端断开了链接\n");
            close(cfd);
            break;
        }
        else {
            printf("recv buf:%s\n",buf);
            send(cfd,buf,recv_len,0);
        }
    }
    printf("Hello world\n");
    return 0;
}

