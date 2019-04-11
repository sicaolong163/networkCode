#include <stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<event2/event.h>
void read_cb(evutil_socket_t fd,short what,void *arg)
{
    //读管道
    char buf[1024]={0};
    int len=read(fd,buf,sizeof(buf));
    printf("data len=%d,buf=%s\n",len,buf);
    printf("read event:%s",what&EV_READ?"yes":"no");
}
//管道读
int main()
{
    unlink("myfifo");
    //创建管道
    mkfifo("myfifo",0664);
    //open file
    int fd=open("myfifo",O_RDONLY|O_NONBLOCK);
    if(fd==-1)
    {
        perror("open error");
        exit(1);
    }
    //读管道
    //1\创建框架
    struct event_base* base=NULL;
    base =event_base_new();
    //2、创建事件
    struct event *ev=NULL;
    //ev=event_new(base,fd,EV_READ|EV_PERSIST,read_cb,NULL);
    ev=event_new(base,fd,EV_READ,read_cb,NULL);
    //3、加入到框架中
    event_add(ev,NULL);
    //4、进入循环事件
    event_base_dispatch(base);
    //5、释放资源
    event_free(ev);
    event_base_free(base);
    close(fd);
    return 0;
}

