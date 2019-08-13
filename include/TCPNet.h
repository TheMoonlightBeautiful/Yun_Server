/*==========================================================================
*   Copyright (C) 2019 jiang. All rights reserved.
*   
*	Authors:	jiangjunwei(1596102356@qq.com)
*	Date:		2019/08/05 11:21:44
*	Description:
*
==========================================================================*/

#pragma once
#include"Yun.h"




class TCPNet
{
private:
	TCPNet();
    static TCPNet* m_pNet;
	~TCPNet();
    
public:
    int m_ser_fd;

public:
    static TCPNet* Get_TCPNet();
    static void  Del_TCPNet();
    int InitSockfd(const char* ip,short host);
    int Accept_Client(int num);
	bool SendData(int socketfd,char* szbuf,int nlen);
	bool RecvData(int socketfd,char* szbuf,int nlen);

};
