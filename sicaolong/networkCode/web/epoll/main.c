#include <stdio.h>
#include<stdlib.h>
#include<unistd.h>


int main(int argc,char **argv)
{
    if(argc<3)
    {
        printf("eg......./a.out port path\n");
        exit(1);
    }
    //端口；给epoll模型用的；
    int port=atoi(argv[1]);
    //x修改进程的工作目录方便后续操作；
    int ret=chdir(argv[2]);
    if(ret==-1)
    {
        perror("chdir error");
        exit(1);
    }

    //启动epoll模型
    
    printf("Hello world\n");
    return 0;
}

