
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
	PackType  m_nType;  //协议
	PFUN      m_pfun;   //协议对应的函数地址
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
	void* GetSqlDBRq(int,char* szbuf);                       //获取数据库请求
    void* InSerFileRq(int,char*szbuf);                       //数据库插入文件请求
    void* DeleteFileRq(int,char*szbuf);                      //数据库删除文件请求
	void* UpLoadFileHeadInfoRq(int,char* szbuf);             //用户上传文件头信息
	void* UpLoadFileInfoRq(int,char* szbuf);                 //获取用户上传的文件块进行保存用户上传文件
	void* UpLoadFileInfoRs(int,char* szbuf);                 //获取用户上传的文件块进行保存用户上传文件
	void* DownFileInfoRq(int,char* szbuf);                   //接收用户下载文件请求

private: 
    bool flgs;
    SQLKernel* p_sql_kernel;
    TCPNet*    p_tcp_net;
};

