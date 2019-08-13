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
	bool IsTableExisted(const string& db_name,const string& table_name);						//���Ƿ����
	bool CreateTable(const string& db_name,const string& sql, const string& table_name);		//������
	bool DropTable(const string& db_name,const string& table_name);							//ɾ����

	bool InsertData(const string& db_name,const string& sql);		//��������
	bool UpdateData(const string& db_name,const string& sql);		//��������
	bool DeleteData(const string& db_name,const string& sql);		//ɾ������

	bool GetDataCount(const string& db_name,const string& sql, int& count);					//��ȡ��¼����
	bool GetDataInfo(const string& db_name,const string& sql, vector<vector<string> >& data);	//��ȡ��¼
	bool GetMaxId(const string& db_name,const string& table_name, int& row_id);				//��ȡ���²������ݵ�ID

	const string& GetLastError() const { return m_ssErrMsg; }			//��ȡ������Ϣ

public:
	void SetDBFileName(const string& file_name,const string& file_path);	//�������ݿ��ļ���
	void SetDBFileFolder(const string& file_path);	//�������ݿ�·��

private:
	MySqlite(const MySqlite&);				//�������캯��ֻ������ʵ��
	MySqlite& operator=(const MySqlite&);	//��ֵ����ֻ������ʵ��

	bool OpenDB(const string& db_name);									//�����ݿ�
	void CloseDB();									//�ر����ݿ�
	bool ExcuteSQL(const string& db_name,const string& sql);				//ִ��SQL���

	static int IsTableExisted(void* para, int n_column, char** column_value, char** column_name);	//�жϱ��Ƿ����

private:
	string	        m_ssErrMsg;			//������Ϣ
	int		        m_snResult;			//������
	sqlite3*		m_pDB;				//���ݿ�ָ��
	string          m_sDBFileName;      //���ݿ�����
	string          m_sDBFilePath;       //���ݿ�·��

};

