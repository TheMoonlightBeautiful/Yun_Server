
/*==========================================================================
*   Copyright (C) 2019 jiang. All rights reserved.
*   
*	Authors:	jiangjunwei(1596102356@qq.com)
*	Date:		2019/08/06 22:09:14
*	Description:
*
==========================================================================*/


#include "MySqlite.h"
#include <sstream>
using std::stringstream;

MySqlite::MySqlite(void)
	: m_pDB(NULL)
	, m_sDBFileName("data.db")
	, m_sDBFilePath("./")
{
	m_sDBFilePath = m_sDBFilePath + m_sDBFileName;
    m_ssErrMsg = "";
    m_snResult = 0;
    m_pDB = NULL;
}

MySqlite::~MySqlite(void)
{

}


//设置数据库文件名
void MySqlite::SetDBFileName(const string& file_path,const string& file_name)
{
	m_sDBFileName = file_name;
    m_sDBFilePath = file_path + m_sDBFileName;
}

//打开数据库，如果该数据库不存在，就创建一个数据库
bool MySqlite::OpenDB(const string& db_name)
{

	m_snResult = sqlite3_open(db_name.c_str(), &m_pDB);
	if (m_snResult != SQLITE_OK)
	{
		stringstream ss;
		ss << "Open database " <<db_name<< " failed.";
		m_ssErrMsg = ss.str();
		return false;
	}
	return true;
}

//关闭数据库
void MySqlite::CloseDB()
{   
	sqlite3_close(m_pDB);
}

//判断表是否存在
//para:传进回调函数的参数,n_column:字段个数
//column_value:char*数组(保存每一个字段的值),column_name:字段名称
int MySqlite::IsTableExisted(void* para, int n_column, char** column_value, char** column_name)
{
	bool* existed = (bool*)para;
	*existed = (**column_value) != '0';	//字段的值为空时表示不存在数据
	return 0;
}

//表是否存在
bool MySqlite::IsTableExisted(const string& db_name,const string& table_name)
{
	//打开数据库
	if (!OpenDB(db_name))
	{
		return false;
	}

	bool existed = false;
	string sql_acsii = "SELECT count(type) FROM sqlite_master WHERE type = 'table' and name = '" + table_name + "'";
	m_snResult = sqlite3_exec(m_pDB, sql_acsii.c_str(), MySqlite::IsTableExisted, &existed, NULL);

	//关闭数据库
	CloseDB();

	return existed;
}

//创建表
bool MySqlite::CreateTable(const string& db_name,const string& sql, const string& table_name)
{
	//判断表是否存在
	if (IsTableExisted(db_name,table_name))
	{
		return true;
	}

	//执行SQL语句
	return ExcuteSQL(db_name,sql);
}

//删除表
bool MySqlite::DropTable(const string& db_name,const string& table_name)
{
	//判断表是否存在
	if (!IsTableExisted(db_name,table_name))
	{
		return true;
	}

	//执行SQL语句
	string sql = "DROP TABLE " + table_name;
	return ExcuteSQL(db_name,sql);
}

//插入数据
bool MySqlite::InsertData(const string& db_name,const string& sql)
{
	return ExcuteSQL(db_name,sql);
}

//删除数据
bool MySqlite::DeleteData(const string& db_name,const string& sql)
{
	return ExcuteSQL(db_name,sql);
}

//更新数据
bool MySqlite::UpdateData(const string& db_name,const string& sql)
{
	return ExcuteSQL(db_name,sql);
}

//执行SQL语句
bool MySqlite::ExcuteSQL(const string& db_name,const string& sql)
{
	//打开数据库
	if (!OpenDB(db_name))
	{
		return false;
	}


	char* pErrMsg = NULL;
	m_snResult = sqlite3_exec(m_pDB, sql.c_str(), NULL, NULL, &pErrMsg);
	m_ssErrMsg = (pErrMsg == NULL ? "" : pErrMsg);
    

	//关闭数据库
	CloseDB();

	return (m_snResult == SQLITE_OK);
}

//获取记录个数
bool MySqlite::GetDataCount(const string& db_name,const string& sql, int& count)
{
	//打开数据库
	if (!OpenDB(db_name))
	{
		return false;
	}

	char* pErrMsg = NULL;
	char** pDBResult = NULL;	//查询结果
	int nColumn = 0;
	int nRow = 0;


	m_snResult = sqlite3_get_table(m_pDB, sql.c_str(), &pDBResult, &nRow, &nColumn, &pErrMsg);
	m_ssErrMsg = (pErrMsg == NULL ? "" : pErrMsg);
	bool ret = (m_snResult == SQLITE_OK);
	if (ret && pDBResult != NULL && pDBResult[nColumn] != NULL)
	{
		count = atoi(pDBResult[nColumn]);
	}

	//释放查询结果(无论查询是否成功)
	sqlite3_free_table(pDBResult);

	//关闭数据库
	CloseDB();

	return ret;
}

//获取记录
bool MySqlite::GetDataInfo(const string& db_name,const string& sql, vector<vector<string> >& data)
{
	//打开数据库
	if (!OpenDB(db_name))
	{
		return false;
	}

	//清空数据
	data.clear();

	char* pErrMsg = NULL;
	char** pDBResult = NULL;	//查询结果,前面是字段名称，后面才是字段的值
	int nRow = 0;
	int nColumn = 0;

	string val;

	m_snResult = sqlite3_get_table(m_pDB, sql.c_str(), &pDBResult, &nRow, &nColumn, &pErrMsg);
	m_ssErrMsg = (pErrMsg == NULL ? "" : pErrMsg);
	bool ret = (m_snResult == SQLITE_OK);
	if (ret)
	{
		int index = nColumn;	//pDBResult前面是列名

		for (int i = 0; i<nRow; i++)
		{
			vector<string> vecRow;
			for (int j = 0; j<nColumn; j++)
			{
				val = pDBResult[index++];
				vecRow.push_back(val);
			}
			data.push_back(vecRow);
		}
	}

	//释放查询结果(无论查询是否成功)
	sqlite3_free_table(pDBResult);

	//关闭数据库
	CloseDB();

	return ret;
}

//获取最新插入数据的ID
bool MySqlite::GetMaxId(const string& db_name,const string& table_name, int& row_id)
{
	bool ret = false;

	//打开数据库
	if (!OpenDB(db_name))
	{
		return ret;
	}

	char* pErrMsg = NULL;
	char** pDBResult = NULL;	//查询结果,前面是字段名称，后面才是字段的值
	int nRow = 0;
	int nColumn = 0;

	string sql_acsii = "SELECT MAX(RID) FROM " + table_name;
	string val;

	m_snResult = sqlite3_get_table(m_pDB, sql_acsii.c_str(), &pDBResult, &nRow, &nColumn, &pErrMsg);
	m_ssErrMsg = (pErrMsg == NULL ? "" : pErrMsg);
	ret = (m_snResult == SQLITE_OK);
	if (ret && pDBResult != NULL && pDBResult[nColumn] != NULL)
	{
		val = pDBResult[nColumn];
		row_id = atoi(val.c_str());
	}

	//释放查询结果(无论查询是否成功)
	sqlite3_free_table(pDBResult);

	//关闭数据库
	CloseDB();

	return ret;
}
