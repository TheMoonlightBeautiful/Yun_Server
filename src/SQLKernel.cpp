
/*==========================================================================
*   Copyright (C) 2019 jiang. All rights reserved.
*   
*	Authors:	jiangjunwei(1596102356@qq.com)
*	Date:		2019/08/06 22:09:14
*	Description:
*
==========================================================================*/


#include "Yun.h"
#include "SQLKernel.h"


SQLKernel::SQLKernel()
{
    m_sql = NULL;
}


SQLKernel::~SQLKernel()
{
    if(m_sql != NULL){
        delete m_sql;
        m_sql = NULL;
    }
		
}


SQLProtocolMap SQLm_ProtocolEntries[] =
{
	{_DEF_REGISTER_RQ,&SQLKernel::RegisterRq},
	{_DEF_LOGIN_RQ,&SQLKernel::LoginRq},
	{_DEF_INSERINTOSQL_RQ ,&SQLKernel::Inser_FileInfo_ToSql},
	{_DEF_DELETEFILEINFOSQL_RQ ,&SQLKernel::Del_FileInfo_ToSql},
	{_DEF_UPLOAD_RS,&SQLKernel::Get_FileInfo_FromSql},     //用户上传完文件发送文件上传完毕回复,函数查询用户个人数据库判断是否完成上传
    {_DEF_FILE_HEADINFO_RQ,&SQLKernel::UpLoadFileHeadInfoRq},
	{ 0,0 }
};


bool SQLKernel::Open(char* host,char* usr,char* password,char* DBname)
{
    m_usr = usr;
    m_host = host;
    m_password = password;
    m_DBname = DBname;

    m_sql = new CMySql;
    if(m_sql == NULL || (!m_sql->ConnectMySql(host,usr,password,DBname)))
    {
        cout<<"Mysql connect failed!"<<endl;
        return -1;
    }else{
        cout<<"Mysql connect seccess!"<<endl;
    }

	return true;
}

void SQLKernel::Close() {


	
}

void* SQLKernel::DealData(char* szbuf) {

	char* pType = (char*)szbuf;
	int i = 0;
	while (1)
	{
		if (SQLm_ProtocolEntries[i].m_nType == *pType)
		{
			return (this->*SQLm_ProtocolEntries[i].m_pFun)(szbuf);
			break;
		}
		else if (SQLm_ProtocolEntries[i].m_nType == 0 && SQLm_ProtocolEntries[i].m_pFun == 0)
			break;

		i++;

	}

	return (void*)NULL;
}


char* SQLKernel::Build_Sqlite_UsrDB(char* usr_id)
{
	MySqlite lite;
    string db_name = string(usr_id);

    char* fun_sql = new char[SQL_SIZE];
    memset(fun_sql,'\0',SQL_SIZE);
    sprintf(fun_sql,"create table %s(RID integer primary key autoincrement,FileName varchar(255),FileType varchar(255),FileSize varchar(255),FileDate datetime,FileMd5 varchar(400),FilePath varchar(255));",USR_SQL_TABLE_NAME);
    
	bool ret = lite.CreateTable(db_name,string(fun_sql),string("FileInfo"));
	if (!ret)
		return NULL;
    char time_buf[DEF_SIZE];
    memset(time_buf,'\0',DEF_SIZE);
    //获取本地时间
    LOCALTIME(time_buf);

    //构造用户Folder -g信息
    memset(fun_sql,'\0',SQL_SIZE);
    sprintf(fun_sql,"insert into FileInfo(FileName,FileType,FileSize,FileDate,FileMd5,FilePath) values('Picture','Folder','NULL','%s','NULL','D');",time_buf);
    lite.InsertData(db_name,string(fun_sql));
    
    memset(fun_sql,'\0',SQL_SIZE);
    sprintf(fun_sql,"insert into FileInfo(FileName,FileType,FileSize,FileDate,FileMd5,FilePath) values('Video','Folder','NULL','%s','NULL','D');",time_buf);
    lite.InsertData(db_name,string(fun_sql));

    memset(fun_sql,'\0',SQL_SIZE);
    sprintf(fun_sql,"insert into FileInfo(FileName,FileType,FileSize,FileDate,FileMd5,FilePath) values('TXT','Folder','NULL','%s','NULL','D');",time_buf);
    lite.InsertData(db_name,string(fun_sql));

    memset(fun_sql,'\0',SQL_SIZE);
    sprintf(fun_sql,"insert into FileInfo(FileName,FileType,FileSize,FileDate,FileMd5,FilePath) values('Resource','Folder','NULL','%s','NULL','D');",time_buf);
    lite.InsertData(db_name,string(fun_sql));

    memset(fun_sql,'\0',SQL_SIZE);
    sprintf(fun_sql,"insert into FileInfo(FileName,FileType,FileSize,FileDate,FileMd5,FilePath) values('collect','Folder','NULL','%s','NULL','D');",time_buf);
    lite.InsertData(db_name,string(fun_sql));


    memset(fun_sql,'\0',SQL_SIZE);
    strcpy(fun_sql,db_name.c_str());
    return fun_sql;
}


void* SQLKernel::RegisterRq(char*szbuf)
{
	STRU_REGISTER_RQ *psrr = (STRU_REGISTER_RQ*)szbuf;
    char fun_sql[SQL_SIZE];
    char time_buf[DEF_SIZE];
    memset(fun_sql,'\0',SQL_SIZE);
    memset(time_buf,'\0',DEF_SIZE);

    //获取本地时间
    LOCALTIME(time_buf);

    //构造用户注册信息
    sprintf(fun_sql,"insert into %s(a_email,a_name,a_sex,a_password,t_brith,t_register,a_sqlpath) values('%s','%s','%s','%s','%s','%s','%s');",USR_INFO_TABLE_NAME,psrr->m_szEmail,psrr->m_szName,"未知",psrr->m_szPassWord,"未知",time_buf,DEF_USRSQLDB_PATH);

    bool fun_flas = m_sql->UpdateMySql(fun_sql);

    //获取用户邮箱作为SQLite数据库名称
    memset(time_buf,'\0',DEF_SIZE);
    strcpy(time_buf,psrr->m_szEmail);
    GET_USR_DB_PATH(time_buf);
    if(strcmp(time_buf,"NULL") == 0)
        return (void*)false;

    char* sql_path = Build_Sqlite_UsrDB(time_buf);
    if(sql_path == NULL)
        fun_flas = false;
    else
        delete[] sql_path;
    
    return (void*)fun_flas;
}

void* SQLKernel::LoginRq(char*szbuf)
{
    STRU_LOGIN_RQ* ssr = (STRU_LOGIN_RQ*)szbuf;
    char fun_sql[SQL_SIZE];
    memset(fun_sql,'\0',SQL_SIZE);
    
    list<string> sql_list;
    
    sprintf(fun_sql,"select a_password from %s where a_email='%s';",USR_INFO_TABLE_NAME,ssr->m_szEmail);

    m_sql->SelectMySql(fun_sql,1,sql_list);    
    
    if(sql_list.size() == 0)
        return (void*)false;
    
    string fun_password = sql_list.front();

    if(strcmp(ssr->m_szPassWord,fun_password.c_str()) == 0)
    {
        return (void*)true;
    }

    return (void*)false;
}

void* SQLKernel::Inser_FileInfo_ToSql(char*szbuf)
{
	STRU_TOSQL_UP_FILEINFO* fifo = (STRU_TOSQL_UP_FILEINFO*)szbuf;
	char buf[DEF_SIZE];

	MySqlite lite;
    cout<<fifo->m_szEmail<<endl;

	strcpy(buf,fifo->m_szEmail);
    GET_USR_DB_PATH(buf); 
    if(strcmp(buf,"NULL") == 0)
        return (void*)false;

	string db_name = buf;       //对应数据库的路径用于后面执行数据库语句

	memset(buf,'\0', DEF_SIZE);
	sprintf(buf,"'%s'", fifo->m_szFileName);
	string filename = buf;              //文件名

	memset(buf, '\0', DEF_SIZE);
	sprintf(buf,"'%s'", fifo->m_szFileType);
	string filetype = buf;              //文件类型

	memset(buf, '\0', DEF_SIZE);
	sprintf(buf,"'%s'", fifo->m_szFileSize);
	string filesize = buf;              //文件大小

	memset(buf, '\0', DEF_SIZE);
	sprintf(buf,"'%s'", fifo->m_szFileUpLoadTime);
	string filedate = buf;                  //文件上传时间

	memset(buf, '\0', DEF_SIZE);
	sprintf(buf,"'%s'", fifo->m_szMD5);
	string filemd5 = buf;                   //文件MD5值

	memset(buf, '\0', DEF_SIZE);
	sprintf(buf,"'%s'", fifo->m_szFilePath);
	string filepath = buf;                      //文件路劲(用户数据库中的文件路径)

	string sql1 = "insert into " + string(USR_SQL_TABLE_NAME) + "(FileName,FileType,FileSize,FileDate,FileMd5,FilePath) Values(" + filename + "," + filetype + "," + filesize + "," + filedate + "," + filemd5 + "," + filepath + ");";
	
	
   bool ret = lite.InsertData(db_name,sql1); 
   return (void*)ret;
}

void* SQLKernel::Get_FileInfo_FromSql(char*szbuf)  //获取用户自己数据库的文件信息
{
//    STRU_GETSQLDB_RS* ssr = (STRU_GETSQLDB_RS*)szbuf;//用户回复上传文件的内容,查询数据库判断是否上传成功,将结果返回给TCPKernel
    


    return (void*)true;
}

void* SQLKernel::Del_FileInfo_ToSql(char*szbuf)
{
    STRU_DELEETEFILEINFO_RQ* ssr = (STRU_DELEETEFILEINFO_RQ*)szbuf;

	MySqlite lite;

	char buf[DEF_SIZE];
	strcpy(buf,ssr->m_szEmail);
    GET_USR_DB_PATH(buf); 
    if(strcmp(buf,"NULL") == 0)
        return (void*)false;
	string db_name = buf;       //对应数据库的路径用于后面执行数据库语句

	string sql;
	char fun_buf[300];
	if(!strcmp(ssr->m_szFatherFilePath,""))
		sprintf(fun_buf,"delete from %s where FileName=\'%s\' and FilePath=\'%s\';",USR_SQL_TABLE_NAME, ssr->m_szFileName, ssr->m_szFilePath);
	else {
		sprintf(fun_buf,"delete from %s where FilePath=\'%s\';", USR_SQL_TABLE_NAME, ssr->m_szFilePath);
		sql = fun_buf;
		if (lite.DeleteData(db_name,sql)) {
			memset(fun_buf,'\0',sizeof(fun_buf));
			sprintf(fun_buf,"delete from %s where FileName=\'%s\' and FilePath=\'%s\';",USR_SQL_TABLE_NAME, ssr->m_szFileName, ssr->m_szFatherFilePath);
		}else
			return (void*)false;
	}

	sql = fun_buf;

	return (void*)lite.DeleteData(db_name,sql);

}


void* SQLKernel::UpLoadFileHeadInfoRq(char* szbuf)             //用户上传文件头信息
{

	STRU_TOSQL_UP_FILEINFO* psrr = (STRU_TOSQL_UP_FILEINFO*)szbuf;
    SQL_FILEINFO_RES   *res = new SQL_FILEINFO_RES;//用户数据库查询信息回复TCPKernel

   //构造数据库语句,判断文件信息 
    char fun_sql[SQL_SIZE];
    memset(fun_sql,'\0',SQL_SIZE);
    char file_path[DEF_SIZE];       //文件在服务器中的路径
    string fun_fileid;    //文件在数据库中的id

    list<string> sql_list;
    
    sprintf(fun_sql,"select RID,FilePath,LoadSize from %s where FileMd5='%s';",LOAD_FILEINFO_TABLE_NAME,psrr->m_szMD5);
    m_sql->SelectMySql(fun_sql,3,sql_list);    
    
    if(sql_list.size() == 0)
    {
        memset(file_path,'\0',DEF_SIZE);
        sprintf(file_path,"%s/%s",DEF_LOADFILE_PATH,psrr->m_szMD5);
        memset(fun_sql,'\0',SQL_SIZE);
        sprintf(fun_sql,"insert into %s(FileName,FileType,FileSize,FileDate,FileMd5,FilePath,LoadUsr,LoadSize,IsLoading) values('%s','%s','%s','%s','%s','%s','%s','%s','%s');",LOAD_FILEINFO_TABLE_NAME,psrr->m_szFileName,psrr->m_szFileType,psrr->m_szFileSize,psrr->m_szFileUpLoadTime,psrr->m_szMD5,file_path,psrr->m_szEmail,"0","yes");
        if(!m_sql->UpdateMySql(fun_sql))
        {
            res->file_id = 0;
            strcpy(res->file_path,"NULL");
            strcpy(res->file_len,"0");
            return (void*)res;
        }
        memset(fun_sql,'\0',SQL_SIZE);
        //查询插入文件信息的id,提高后续用户上传信息
        sprintf(fun_sql,"select RID from %s where FileMd5='%s';",LOAD_FILEINFO_TABLE_NAME,psrr->m_szMD5);
        m_sql->SelectMySql(fun_sql,1,sql_list);    
        fun_fileid = sql_list.front();
        
        res->file_id = atoi(fun_fileid.c_str());
        strcpy(res->file_path,file_path);
        strcpy(res->file_len,"0");

        return (void*)res;
    }else{
        fun_fileid = sql_list.front();   
        sql_list.pop_front();
        res->file_id = atoi(fun_fileid.c_str());  //id

        fun_fileid = sql_list.front();
        sql_list.pop_front();
        strcpy(res->file_path,fun_fileid.c_str());   //路径
        
        fun_fileid = sql_list.front();   
        sql_list.pop_front();
        if(!strcmp(fun_fileid.c_str(),psrr->m_szFileSize))
        {
            strcpy(res->file_len,fun_fileid.c_str());   //文件中存在的大小

        }else
        {   
            int file_len = 0;
            FILE* file_fd = fopen(res->file_path,"r");
            fseek(file_fd,0,SEEK_END);
            file_len = ftell(file_fd);
            memset(fun_sql,'\0',SQL_SIZE);
            sprintf(res->file_len,"%d",file_len);
            //更新数据库文件信息
            if(strcmp(res->file_len,fun_fileid.c_str()) != 0)
            {
                sprintf(fun_sql,"update %s set LoadSize='%s' where FileMd5='%s';",LOAD_FILEINFO_TABLE_NAME,res->file_len,psrr->m_szMD5);
                m_sql->UpdateMySql(fun_sql);   
            }

        }

        return (void*)res;

    }
    
}
