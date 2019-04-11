#pragma once
#ifndef __EPOLL_SERVER_H
#define __EPOLL_SERVER_H
int init_listen(int ,int );
void do_accept(int ,int );
void epoll_run(int );
int get_line(int sock, char *buf, int size);
void disconnect(int ,int);
void do_read(int,int);
#endif

