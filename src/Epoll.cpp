/*==========================================================================
*   Copyright (C) 2019 jiang. All rights reserved.
*   
*	Authors:	jiangjunwei(1596102356@qq.com)
*	Date:		2019/08/06 22:09:14
*	Description:
*
==========================================================================*/


#include"Epoll.h"





Epoll::Epoll()
{
    m_epoll_fd = 0;
    m_epoll_num = 0;

}

Epoll::~Epoll()
{

}

bool Epoll::Epoll_Create()
{
    m_epoll_fd = epoll_create(EPOLLMAX);

    if(-1 == m_epoll_fd)
    {
        return false;
    }

    return true;
}

bool Epoll::Epoll_Add_fd(int sockfd)
{
    if(-1 == m_epoll_fd)
        return false;

	struct epoll_event epen;
	epen.data.fd = sockfd;
	epen.events = EPOLLIN|EPOLLET;

	if(epoll_ctl(m_epoll_fd,EPOLL_CTL_ADD,sockfd,&epen) == -1)
	{
		return false;
	}
   
    m_epoll_num++;
    return true;
}
    
bool Epoll::Epoll_Del_fd(int sockfd)
{
	if(epoll_ctl(m_epoll_fd,EPOLL_CTL_DEL,sockfd,NULL) == -1)
	{
		return false;
	}

    m_epoll_num++;
    return true;

}
    
int Epoll::Epoll_Wait()
{
    int num = epoll_wait(m_epoll_fd,ep_even,EPOLLMAX,-1);
    return num;
}





