
/*==========================================================================
*   Copyright (C) 2019 jiang. All rights reserved.
*   
*	Authors:	jiangjunwei(1596102356@qq.com)
*	Date:		2019/08/06 22:09:14
*	Description:
*
==========================================================================*/


#include"Yun.h"
#include"Epoll.h"
#include"TCPNet.h"
#include"TCPKernel.h"
#include"Thread.h"


FILE* log_fd = NULL; 
Thread* pthread = NULL;

int main()
{

  
/*	pid_t pid,sid;

	pid = fork();

	if(pid > 0)
	{
		exit(-1);
	}else if(pid < 0)
	{
		exit(-1);
	}

	umask(0);

	sid = setsid();

	if(sid < 0)
	{
		exit(-1);
	}

	if(chdir("/")<0)
	{
		exit(-1);
	}

	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);*/

    pthread = new Thread(10,100,10);
    pthread->Thread_Pool_Create();
    //pthread->Thread_Task_Add(pthread->Thread_Usr_Job,NULL);
    

    Epoll* pepoll = new Epoll;
    TCPNet* ptcp = TCPNet::Get_TCPNet();
	ptcp->m_ser_fd = ptcp->InitSockfd(SERVERIP,SERVERPORT);

	if(ptcp->m_ser_fd == -1) 
        return-1;

    TCPKernel* tcp_kernel = new TCPKernel;
    tcp_kernel->Open((char*)"localhost",(char*)"root",(char*)"Jiang123",(char*)"yun");

	
	listen(ptcp->m_ser_fd,EPOLLMAX);
	
    if(!pepoll->Epoll_Create())
    {
        cout<<"Epoll Init Failed"<<endl;
        return -1;
    }

	if(!pepoll->Epoll_Add_fd(ptcp->m_ser_fd)) 
    {
        cout<<"Epoll Add Ser_fd Failed"<<endl;
        return -1;
    }

    cout<<"Server Working..."<<endl;


	int i = 0;
    int alread_fd = 0;
    int cli_fd = 0;
    char* msg_buf = NULL;
    int msg_len = 0;

	while(1)
	{
		alread_fd = pepoll->Epoll_Wait();

		for(i=0;i<alread_fd;i++)
		{

			if(pepoll->ep_even[i].data.fd == ptcp->m_ser_fd && pepoll->ep_even[i].events == EPOLLIN)
			{
				if(pepoll->m_epoll_num < EPOLLMAX)
				{
					log_fd = fopen("ser_date.log","a+");
                    cli_fd = ptcp->Accept_Client(pepoll->m_epoll_num);
                    if(cli_fd != -1)
                    {
                        pepoll->Epoll_Add_fd(cli_fd);
                    }

					fclose(log_fd);
					
				}else
				{
					cout<<"EVENTS FULL"<<endl;
					continue;
				}
			} else if(pepoll->ep_even[i].data.fd != ptcp->m_ser_fd && pepoll->ep_even[i].events == EPOLLIN)
			{
                bool boread = ptcp->RecvData(pepoll->ep_even[i].data.fd,(char*)&msg_len,sizeof(int));
				if(!boread)
				{	
					int clifd = pepoll->ep_even[i].data.fd;
                    pepoll->Epoll_Del_fd(clifd);
					close(clifd);
					continue;
				}
                
                if(msg_len > 0){
                    msg_buf = new char[msg_len];
                    ptcp->RecvData(pepoll->ep_even[i].data.fd,msg_buf,msg_len);
                    pthread->Thread_Task_Add(tcp_kernel->DealData,tcp_kernel,pepoll->ep_even[i].data.fd,msg_buf);
                    delete msg_buf;
                    msg_buf = NULL;
                    msg_len = 0;
                }

			}


		}


	}

	return 0;
}
