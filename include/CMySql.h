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



class CMySql
{
public:
    CMySql(void);
    ~CMySql(void);
public:
    bool  ConnectMySql(char *host,char *user,char *pass,char *db);
    void  DisConnect();
    bool  SelectMySql(char* szSql,int nColumn,list<string>& lstStr);
   
    //更新：删除、插入、修改
    bool  UpdateMySql(char* szSql);
 
private:
    MYSQL *sock;   
	MYSQL_RES *results;   
	MYSQL_ROW record; 
};   

