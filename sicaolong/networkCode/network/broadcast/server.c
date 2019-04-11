#include <stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<error.h>
#include<string.h>
#include<net/if.h>
int main(int argc,char **argv)
{
    if(argc<2)
    {
        printf("输入参数的个数不匹配，请重新输入\n");
        exit(1);
    }
    int port=atoi(argv[1]);
    //1、创建套接字
    int fd=socket(AF_INET,SOCK_DGRAM,0);
    if(fd==-1)
    {
        perror("socket error");
        exit(1);
    }
    //初始化 服务端；
    struct sockaddr_in serv_addr;
    socklen_t serv_len=sizeof(serv_addr);
    memset(&serv_addr,0,serv_len);
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(port);
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);

    //2、绑定服务器的ip和端口
    int ret=bind(fd,(struct sockaddr*)&serv_addr, serv_len);
    if(ret==-1)
    {
        perror("bind error");
        exit(1);
    }
    //初始化客户端地址信息；
    struct sockaddr_in client_addr;
    memset(&client_addr,0,sizeof(client_addr));
    client_addr.sin_family=AF_INET;
    client_addr.sin_port=htons(8888);
    //使用广播给客户端发送数据
    inet_pton(AF_INET,"239.0.0.10",&client_addr.sin_addr.s_addr);
    
    //给服务器 开放组播权限；
    struct ip_mreqn flags;
    inet_pton(AF_INET,"239.0.0.10",&flags.imr_multiaddr.s_addr);
    inet_pton(AF_INET,"0.0.0.0",&flags.imr_address.s_addr);
    flags.imr_ifindex=if_nametoindex("eth0");
    setsockopt(fd,IPPROTO_IP,IP_MULTICAST_IF,&flags,sizeof(flags));
    
    //3、通信
    while(1)
    {
        //一直给客户端发送数据；
        static int num=0;
        char buf[1024];
         sprintf(buf,"hello udp===%d\n",num++);
        int ret=sendto(fd,buf,strlen(buf)+1,0,(struct sockaddr*)&client_addr,sizeof(client_addr));
        if(ret==-1)
        {
            perror("sendto error");
            break;
        }
        printf("server ===send buf:%s\n",buf);
        sleep(1);
    }
    //4、关闭套接字
    printf("Hello world\n");
    return 0;
}

