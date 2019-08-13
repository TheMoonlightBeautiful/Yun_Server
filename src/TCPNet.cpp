
/*==========================================================================
*   Copyright (C) 2019 jiang. All rights reserved.
*   
*	Authors:	jiangjunwei(1596102356@qq.com)
*	Date:		2019/08/06 22:09:14
*	Description:
*
==========================================================================*/




#include"TCPNet.h"


extern FILE* log_fd;


TCPNet* TCPNet::m_pNet = NULL;

TCPNet::TCPNet()
{
    m_ser_fd = 0;

}

TCPNet::~TCPNet()
{

}

TCPNet* TCPNet::Get_TCPNet()
{
     if(NULL == m_pNet)
     {
          m_pNet = new TCPNet();
     }

     return m_pNet;
}

void TCPNet::Del_TCPNet()
{
    if(NULL != m_pNet)
        delete m_pNet;
}

int TCPNet::Accept_Client(int num)
{
	char ip[16];
	struct sockaddr_in cli_addr;
	int cli_len = sizeof(cli_addr);

	int fd = accept(m_ser_fd,(struct sockaddr*)&cli_addr,(socklen_t*)&cli_len);

    if(-1 == fd)
        return -1;

	inet_ntop(AF_INET,(struct sockaddr*)&cli_addr.sin_addr.s_addr,ip,16);

	time_t t;
	
	time(&t);
	fprintf(log_fd,"time:%s\tclient ip:%s\taccept All client num:%d\n",ctime(&t),inet_ntop(AF_INET,(struct sockaddr*)&cli_addr.sin_addr.s_addr,ip,16),num);
	
	
    return fd;
}

bool TCPNet::SendData(int socketfd,char* szbuf,int nlen)
{
    int len = 0;
    int numlen = 0;

    write(socketfd,(char*)&nlen,sizeof(int));

    while(numlen < nlen)
    {
         len = write(socketfd,szbuf+numlen,nlen-numlen);
         numlen += len;
    }

    return true;
}

bool TCPNet::RecvData(int socketfd,char* szbuf,int nlen)
{
    int len = 0;
    int numlen = 0;

    while(numlen < nlen)
    {
        len = read(socketfd,szbuf+numlen,nlen-numlen);
        if(len <= 0)
            return false;

        numlen += len;
    }

    return true;

}

int TCPNet::InitSockfd(const char* ip,short host)
{

	if(ip == NULL)return -1;


	struct sockaddr_in ser_addr;
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_addr.s_addr = inet_addr(ip);
	ser_addr.sin_port = htons(host);

	int fd = socket(AF_INET,SOCK_STREAM,0);

	if(bind(fd,(struct sockaddr*)&ser_addr,sizeof(ser_addr)) == -1)
	{
		return -1;
	}


	return fd;
}
