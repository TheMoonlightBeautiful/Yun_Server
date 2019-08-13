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
	PackType m_nType;  //Э��
	SQLPFUN	 m_pFun;   //Э���Ӧ�ĺ�����ַ
};

class SQLKernel
{
public:
	SQLKernel();
	~SQLKernel();

public:
	bool Open(char* host,char* usr,char* password,char* DBname);
	void Close();
    char* Build_Sqlite_UsrDB(char* usr_id);   //�����û����ݿ�
	void* DealData(char* szbuf);
	
public:
	void* Inser_FileInfo_ToSql(char*szbuf);
	void* Get_FileInfo_FromSql(char*szbuf);  //���������ڻ�ȡ�û����ݿ��Ʒ�������Ϣ,���û����Ͳ����ļ��Ƿ�ɹ���Ϣ
	void* Del_FileInfo_ToSql(char*szbuf);
	void* RegisterRq(char*szbuf);
	void* LoginRq(char*szbuf);
	void* UpLoadFileHeadInfoRq(char* szbuf);             //�û��ϴ��ļ�ͷ��Ϣ
	
private:
    CMySql* m_sql;
    string m_host;
    string m_usr;
    string m_password;
    string m_DBname;

};

