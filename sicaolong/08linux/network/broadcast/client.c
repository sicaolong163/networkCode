#include <stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<net/if.h>
int main(int argc,char **argv)
{
    //1、创建套接字
    int fd=socket(AF_INET,SOCK_DGRAM,0);
    if(fd==-1)
    {
        perror("socket error");
        exit(1);

    }
    //绑定ip地址和端口；
    struct sockaddr_in client_addr;
    socklen_t cli_len=sizeof(client_addr);
    memset(&client_addr,0,cli_len);
    client_addr.sin_family=AF_INET;
    client_addr.sin_port=htons(8888);//一定要把对应的端口设置对；
    //0.0.0.0 会自动适配；客户端的ip地址不知道；
    inet_pton(AF_INET,"0.0.0.0",&client_addr.sin_addr.s_addr);

    int ret=bind(fd,(struct sockaddr*)&client_addr,cli_len);
    if(ret==-1)
    {
        printf("bind error");
        exit(1);
    }
    //加入到组播地址
    //加入到组播地址
    //加入到组播地址
    //加入到组播地址
    //加入到组播地址
    struct ip_mreqn flag;
    inet_pton(AF_INET,"224.0.1.255",&flag.imr_multiaddr.s_addr);
    inet_pton(AF_INET,"0.0.0.0",&flag.imr_address.s_addr);
    flag.imr_ifindex = if_nametoindex("eth0");
    setsockopt(fd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&flag,sizeof(flag));
    //3、接收数据
    while(1)
    {
        char buf[1024]={0};
        int len=recvfrom(fd,buf,sizeof(buf),0,NULL,NULL);
        if(len==-1)
        {
            perror("recvfrom error");
            exit(1);
        }
        printf("client =====recv buf: %s\n",buf);
    }
    close(fd);
    printf("Hello world\n");
    return 0;
}

