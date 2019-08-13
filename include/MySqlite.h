#pragma once
#include"Yun.h"
#include <stdlib.h>  
#include "sqlite3.h"  
#include <vector>
#include <string>

using std::string;
using std::vector;

class MySqlite
{
public:
	MySqlite(void);
	~MySqlite();

public:
	bool IsTableExisted(const string& db_name,const string& table_name);						//表是否存在
	bool CreateTable(const string& db_name,const string& sql, const string& table_name);		//创建表
	bool DropTable(const string& db_name,const string& table_name);							//删除表

	bool InsertData(const string& db_name,const string& sql);		//插入数据
	bool UpdateData(const string& db_name,const string& sql);		//更新数据
	bool DeleteData(const string& db_name,const string& sql);		//删除数据

	bool GetDataCount(const string& db_name,const string& sql, int& count);					//获取记录个数
	bool GetDataInfo(const string& db_name,const string& sql, vector<vector<string> >& data);	//获取记录
	bool GetMaxId(const string& db_name,const string& table_name, int& row_id);				//获取最新插入数据的ID

	const string& GetLastError() const { return m_ssErrMsg; }			//获取错误信息

public:
	void SetDBFileName(const string& file_name,const string& file_path);	//设置数据库文件名
	void SetDBFileFolder(const string& file_path);	//设置数据库路径

private:
	MySqlite(const MySqlite&);				//拷贝构造函数只声明不实现
	MySqlite& operator=(const MySqlite&);	//赋值函数只声明不实现

	bool OpenDB(const string& db_name);									//打开数据库
	void CloseDB();									//关闭数据库
	bool ExcuteSQL(const string& db_name,const string& sql);				//执行SQL语句

	static int IsTableExisted(void* para, int n_column, char** column_value, char** column_name);	//判断表是否存在

private:
	string	        m_ssErrMsg;			//错误信息
	int		        m_snResult;			//错误码
	sqlite3*		m_pDB;				//数据库指针
	string          m_sDBFileName;      //数据库名字
	string          m_sDBFilePath;       //数据库路径

};

