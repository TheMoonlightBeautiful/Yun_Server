
/*==========================================================================
*   Copyright (C) 2019 jiang. All rights reserved.
*   
*	Authors:	jiangjunwei(1596102356@qq.com)
*	Date:		2019/08/06 22:09:14
*	Description:
*
==========================================================================*/



#pragma once



#define _DEFSIZE     1024
#define _DEF_STREAMSIZE		552
#define _DEF_STREAMNUM		1000



//边界值
#define SQL_SIZE        300
#define DEF_SIZE         50
#define DEF_NUM         10
#define DEF_SQLLEN       300
#define MAX_CONTENTNUM      2000


#define _DEF_BASE		100


//验证码请求
//#define _DEF_EMAIL_RQ						_DEF_BASE+1     
//#define _DEF_EMAIL_RQ						_DEF_BASE+2

//注册
#define _DEF_REGISTER_RQ					_DEF_BASE+3     
#define _DEF_REGISTER_RS					_DEF_BASE+4

//登陆
#define _DEF_LOGIN_RQ						_DEF_BASE+5
#define _DEF_LOGIN_RS						_DEF_BASE+6

//文件头信息
#define _DEF_FILE_HEADINFO_RQ				_DEF_BASE+7
#define _DEF_FILE_HEADINFO_RS			    _DEF_BASE+8

//上传下载文件
#define _DEF_UPLOAD_RQ					    _DEF_BASE+9
#define _DEF_UPLOAD_RS					    _DEF_BASE+10


//向本地数据库获取指定文件目录下文件信息
#define _DEF_GETFILEINFOSQL_RQ			    _DEF_BASE+11

//数据库上传文件信息
#define _DEF_INSERINTOSQL_RQ			    _DEF_BASE+12
#define _DEF_INSERINTOSQL_RS			    _DEF_BASE+13

//删除数据库中的文件信息 
#define _DEF_DELETEFILEINFOSQL_RQ		    _DEF_BASE+14
#define _DEF_DELETEFILEINFOSQL_RS		    _DEF_BASE+15

//向服务器获取数据库
#define _DEF_GETSQLDB_RQ					_DEF_BASE+16
#define _DEF_GETSQLDB_RS				    _DEF_BASE+17

#define _DEF_UPLOAD_END					    _DEF_BASE+18        //上传文件完成

//上传下载文件
#define _DEF_DOWN_RQ					    _DEF_BASE+19
#define _DEF_DOWN_RS					    _DEF_BASE+20



//回复结果
//注册结果
#define register_fail	0
#define register_success	1

//登陆结果
#define login_fail	 0
#define login_success	1

//删除文件结果
#define delete_file_fail	 0
#define delete_file_success	1


typedef char PackType;

//验证码请求回复
typedef struct
{
	PackType nType;
	char szMail[DEF_SIZE];
}mail_varify_req;

typedef struct
{
	PackType nType;
	char szCode[DEF_SIZE];
}mail_varify_res;

//注册请求包
struct STRU_REGISTER_RQ
{
	PackType m_nType;
	char	m_szEmail[DEF_SIZE];
	char	m_szName[DEF_SIZE];
	char	m_szPassWord[DEF_SIZE];
};

//登陆请求包
struct STRU_LOGIN_RQ
{
	PackType m_nType;
	char	m_szEmail[DEF_SIZE];
	char	m_szPassWord[DEF_SIZE];
};

//注册(登陆)回复协议包
typedef struct STRU_REGISTER_RS
{
	PackType m_nType;
	char    m_szDBName[DEF_SIZE];  //数据库名字
	char	m_szResult;
}STRU_LOGIN_RS;


//获取文件目录下信息（客户端单独使用）
struct STRU_GETFILEINFO
{
	PackType m_nType;
	char     m_szFilePath[DEF_SIZE];
};

//文件数据库存储信息(用于上传文件或者客户端用于从本地数据库获取文件信息)
struct STRU_TOSQL_UP_FILEINFO
{
	PackType m_nType;
	char	 m_szEmail[DEF_SIZE];
	char     m_szFileName[DEF_SIZE];
	char     m_szFileType[DEF_SIZE]; //FAILED表示上传文件失败
	char     m_szFileSize[DEF_SIZE];
	char     m_szFileUpLoadTime[DEF_SIZE];
	char     m_szMD5[DEF_SIZE];  //生成文件唯一标识-校验文件是否完全相同
	char     m_szFilePath[DEF_SIZE];
};


//上传文件回复
struct STRU_TOSQL_UP_FILEINFO_RS
{
	PackType m_nType;
    int      m_id;                  //文件id
	char	 m_szEmail[DEF_SIZE];
	char     m_szFileName[DEF_SIZE];
	char     m_szFileType[DEF_SIZE];
	char     m_szFileSize[DEF_SIZE];
	char     m_szFileUpLoadTime[DEF_SIZE];
	char     m_szMD5[DEF_SIZE];  //生成文件唯一标识-校验文件是否完全相同
	char     m_szFilePath[DEF_SIZE];
	char     m_szResult[DEF_SIZE];    //1表示成功,0表示失败其他数字表示文件在服务器中的已存在大小,用于断点续传
};


//删除文件信息请求
struct STRU_DELEETEFILEINFO_RQ
{
	PackType m_nType;
	char	 m_szEmail[DEF_SIZE];
	char     m_szFileName[DEF_SIZE];
	char     m_szFatherFilePath[DEF_SIZE];
	char     m_szFilePath[DEF_SIZE];
};


//删除文件回复
struct STRU_DELEETEFILEINFO_RS
{
	PackType m_nType;
	char	 m_szEmail[DEF_SIZE];
	char     m_szFileName[DEF_SIZE];
	char     m_szFatherFilePath[DEF_SIZE];
	char     m_szFilePath[DEF_SIZE];
	char     m_szResult;    //1表示成功
};


//获取数据库请求
struct STRU_GETSQLDB_RQ
{
	PackType m_nType;
	char     m_szEmail[DEF_SIZE];
};

//获取数据库回复
struct STRU_GETSQLDB_RS
{
	PackType m_nType;
	char    m_szDBName[DEF_SIZE];  //数据库名字
	char    m_szDBPath[DEF_SIZE];  //数据库在服务器路径
	int     m_nlen;    //数据库长度,判断其是否全部接收完
};

/*收取回复之间先接受数据库文件由文件传输结构体接收*/
//文件传输
typedef struct STRU_UPLOADFILE
{
	PackType m_nType;
    int      m_fd;                  //文件id
	char     m_szName[DEF_SIZE];     //文件名,用户给服务发文件时填写由服务器给以的文件路径
	unsigned char     m_szContent[MAX_CONTENTNUM];
    int      m_readLen;             //已经传送大小
	int      m_nLen;               //该包文件内容大小
}STRU_DOWNFILE;         //下载文件

typedef struct STRU_UPLOADFILE_RQ
{
	PackType m_nType;
    int      m_fd;                  //文件id
	char     m_szName[DEF_SIZE];     //文件名
	int      m_pos;               //想要的文件位置
}STRU_DOWNFILE_RQ;          //下载文件

