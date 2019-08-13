
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


//�������ݿ��ļ���
void MySqlite::SetDBFileName(const string& file_path,const string& file_name)
{
	m_sDBFileName = file_name;
    m_sDBFilePath = file_path + m_sDBFileName;
}

//�����ݿ⣬��������ݿⲻ���ڣ��ʹ���һ�����ݿ�
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

//�ر����ݿ�
void MySqlite::CloseDB()
{   
	sqlite3_close(m_pDB);
}

//�жϱ��Ƿ����
//para:�����ص������Ĳ���,n_column:�ֶθ���
//column_value:char*����(����ÿһ���ֶε�ֵ),column_name:�ֶ�����
int MySqlite::IsTableExisted(void* para, int n_column, char** column_value, char** column_name)
{
	bool* existed = (bool*)para;
	*existed = (**column_value) != '0';	//�ֶε�ֵΪ��ʱ��ʾ����������
	return 0;
}

//���Ƿ����
bool MySqlite::IsTableExisted(const string& db_name,const string& table_name)
{
	//�����ݿ�
	if (!OpenDB(db_name))
	{
		return false;
	}

	bool existed = false;
	string sql_acsii = "SELECT count(type) FROM sqlite_master WHERE type = 'table' and name = '" + table_name + "'";
	m_snResult = sqlite3_exec(m_pDB, sql_acsii.c_str(), MySqlite::IsTableExisted, &existed, NULL);

	//�ر����ݿ�
	CloseDB();

	return existed;
}

//������
bool MySqlite::CreateTable(const string& db_name,const string& sql, const string& table_name)
{
	//�жϱ��Ƿ����
	if (IsTableExisted(db_name,table_name))
	{
		return true;
	}

	//ִ��SQL���
	return ExcuteSQL(db_name,sql);
}

//ɾ����
bool MySqlite::DropTable(const string& db_name,const string& table_name)
{
	//�жϱ��Ƿ����
	if (!IsTableExisted(db_name,table_name))
	{
		return true;
	}

	//ִ��SQL���
	string sql = "DROP TABLE " + table_name;
	return ExcuteSQL(db_name,sql);
}

//��������
bool MySqlite::InsertData(const string& db_name,const string& sql)
{
	return ExcuteSQL(db_name,sql);
}

//ɾ������
bool MySqlite::DeleteData(const string& db_name,const string& sql)
{
	return ExcuteSQL(db_name,sql);
}

//��������
bool MySqlite::UpdateData(const string& db_name,const string& sql)
{
	return ExcuteSQL(db_name,sql);
}

//ִ��SQL���
bool MySqlite::ExcuteSQL(const string& db_name,const string& sql)
{
	//�����ݿ�
	if (!OpenDB(db_name))
	{
		return false;
	}


	char* pErrMsg = NULL;
	m_snResult = sqlite3_exec(m_pDB, sql.c_str(), NULL, NULL, &pErrMsg);
	m_ssErrMsg = (pErrMsg == NULL ? "" : pErrMsg);
    

	//�ر����ݿ�
	CloseDB();

	return (m_snResult == SQLITE_OK);
}

//��ȡ��¼����
bool MySqlite::GetDataCount(const string& db_name,const string& sql, int& count)
{
	//�����ݿ�
	if (!OpenDB(db_name))
	{
		return false;
	}

	char* pErrMsg = NULL;
	char** pDBResult = NULL;	//��ѯ���
	int nColumn = 0;
	int nRow = 0;


	m_snResult = sqlite3_get_table(m_pDB, sql.c_str(), &pDBResult, &nRow, &nColumn, &pErrMsg);
	m_ssErrMsg = (pErrMsg == NULL ? "" : pErrMsg);
	bool ret = (m_snResult == SQLITE_OK);
	if (ret && pDBResult != NULL && pDBResult[nColumn] != NULL)
	{
		count = atoi(pDBResult[nColumn]);
	}

	//�ͷŲ�ѯ���(���۲�ѯ�Ƿ�ɹ�)
	sqlite3_free_table(pDBResult);

	//�ر����ݿ�
	CloseDB();

	return ret;
}

//��ȡ��¼
bool MySqlite::GetDataInfo(const string& db_name,const string& sql, vector<vector<string> >& data)
{
	//�����ݿ�
	if (!OpenDB(db_name))
	{
		return false;
	}

	//�������
	data.clear();

	char* pErrMsg = NULL;
	char** pDBResult = NULL;	//��ѯ���,ǰ�����ֶ����ƣ���������ֶε�ֵ
	int nRow = 0;
	int nColumn = 0;

	string val;

	m_snResult = sqlite3_get_table(m_pDB, sql.c_str(), &pDBResult, &nRow, &nColumn, &pErrMsg);
	m_ssErrMsg = (pErrMsg == NULL ? "" : pErrMsg);
	bool ret = (m_snResult == SQLITE_OK);
	if (ret)
	{
		int index = nColumn;	//pDBResultǰ��������

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

	//�ͷŲ�ѯ���(���۲�ѯ�Ƿ�ɹ�)
	sqlite3_free_table(pDBResult);

	//�ر����ݿ�
	CloseDB();

	return ret;
}

//��ȡ���²������ݵ�ID
bool MySqlite::GetMaxId(const string& db_name,const string& table_name, int& row_id)
{
	bool ret = false;

	//�����ݿ�
	if (!OpenDB(db_name))
	{
		return ret;
	}

	char* pErrMsg = NULL;
	char** pDBResult = NULL;	//��ѯ���,ǰ�����ֶ����ƣ���������ֶε�ֵ
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

	//�ͷŲ�ѯ���(���۲�ѯ�Ƿ�ɹ�)
	sqlite3_free_table(pDBResult);

	//�ر����ݿ�
	CloseDB();

	return ret;
}
