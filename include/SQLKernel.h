/*==========================================================================
*   Copyright (C) 2019 jiang. All rights reserved.
*   
*	Authors:	jiangjunwei(1596102356@qq.com)
*	Date:		2019/08/05 11:26:02
*	Description:
*
==========================================================================*/

#pragma once
#include"Yun.h"
#include<string>
#include"MySqlite.h"
#include"CMySql.h"
using namespace std;

class SQLKernel;


typedef void* (SQLKernel::*SQLPFUN)(char*);

struct SQLProtocolMap
{
	PackType m_nType;  //协议
	SQLPFUN	 m_pFun;   //协议对应的函数地址
};

class SQLKernel
{
public:
	SQLKernel();
	~SQLKernel();

public:
	bool Open(char* host,char* usr,char* password,char* DBname);
	void Close();
    char* Build_Sqlite_UsrDB(char* usr_id);   //构造用户数据库
	void* DealData(char* szbuf);
	
public:
	void* Inser_FileInfo_ToSql(char*szbuf);
	void* Get_FileInfo_FromSql(char*szbuf);  //服务器用于获取用户数据库云服务器信息,向用户发送插入文件是否成功信息
	void* Del_FileInfo_ToSql(char*szbuf);
	void* RegisterRq(char*szbuf);
	void* LoginRq(char*szbuf);
	void* UpLoadFileHeadInfoRq(char* szbuf);             //用户上传文件头信息
	
private:
    CMySql* m_sql;
    string m_host;
    string m_usr;
    string m_password;
    string m_DBname;

};

