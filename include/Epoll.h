/*==========================================================================
*   Copyright (C) 2019 jiang. All rights reserved.
*   
*	Authors:	jiangjunwei(1596102356@qq.com)
*	Date:		2019/08/05 11:23:22
*	Description:
*
==========================================================================*/

#pragma once
#include"Yun.h"





class Epoll
{
public:
    Epoll();
    ~Epoll();

public:
    bool Epoll_Create();
    bool Epoll_Add_fd(int sockfd);
    bool Epoll_Del_fd(int sockfd);
    int Epoll_Wait();

private:
    int m_epoll_fd;

public:
    struct epoll_event ep_even[EPOLLMAX];
    int m_epoll_num;

};
