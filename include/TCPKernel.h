
/*==========================================================================
*   Copyright (C) 2019 jiang. All rights reserved.
*   
*	Authors:	jiangjunwei(1596102356@qq.com)
*	Date:		2019/08/06 22:09:14
*	Description:
*
==========================================================================*/




#pragma once
#include "IKernel.h"
#include "TCPNet.h"
#include "SQLKernel.h"

class TCPKernel;

typedef void* (TCPKernel::*PFUN)(int ,char*) ;
struct ProtocolMap
{
	PackType  m_nType;  //Э��
	PFUN      m_pfun;   //Э���Ӧ�ĺ�����ַ
};

class  TCPKernel/*: public IKernel*/
{
public:
	TCPKernel();
	~TCPKernel();
public:
	bool Open(char* host,char* usr,char* password,char* DBname);
	void Close();
	static void* DealData(void* ptcp,int sockfd,char* szbuf);
	
public:
	void* RegisterRq(int,char* szbuf);
	void* LoginRq(int,char* szbuf);
	void* GetSqlDBRq(int,char* szbuf);                       //��ȡ���ݿ�����
    void* InSerFileRq(int,char*szbuf);                       //���ݿ�����ļ�����
    void* DeleteFileRq(int,char*szbuf);                      //���ݿ�ɾ���ļ�����
	void* UpLoadFileHeadInfoRq(int,char* szbuf);             //�û��ϴ��ļ�ͷ��Ϣ
	void* UpLoadFileInfoRq(int,char* szbuf);                 //��ȡ�û��ϴ����ļ�����б����û��ϴ��ļ�
	void* UpLoadFileInfoRs(int,char* szbuf);                 //��ȡ�û��ϴ����ļ�����б����û��ϴ��ļ�
	void* DownFileInfoRq(int,char* szbuf);                   //�����û������ļ�����

private: 
    bool flgs;
    SQLKernel* p_sql_kernel;
    TCPNet*    p_tcp_net;
};

