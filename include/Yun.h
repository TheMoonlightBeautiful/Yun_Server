#pragma once
#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<list>
#include<mysql/mysql.h>
#include<string>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/epoll.h>
#include<regex.h>
#include<signal.h>
#include<errno.h>
#include<arpa/inet.h>
#include<sys/mman.h>
#include<netdb.h>
#include<time.h>
#include<pthread.h>
#include<ctype.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sqlite3.h>
#include<netinet/in.h>
#include"Packdef.h"
using namespace std;

#define SERVERIP "192.168.43.2"
//#define SERVERIP "0.0.0.0"
//#define SERVERIP "127.0.0.1"
#define SERVERPORT 8000

#define EPOLLMAX 1000
#define TIMEOUT  30


#define DEF_LOADFILE_PATH                   "LoadFilePath"         //用户上传文件保存目录
#define DEF_USRSQLDB_PATH                   "UsrSQLDBPath"         //用户数据哭保存目录
#define USR_INFO_TABLE_NAME                 "usrinfo"              //Mysql用户信息表名字
#define LOAD_FILEINFO_TABLE_NAME            "FileInfo"             //Mysql文件信息表名称
#define USR_SQL_TABLE_NAME                  "FileInfo"             //SQlite用户文件信息表名称


struct SQL_FILEINFO_RES //用户数据库查询信息回复TCPKernel
{
    int  file_id;                   //文件在数据库中id,用于后续用户快速找到文件信息
    char file_path[DEF_SIZE];       //文件在服务器中的路径
    char file_len[DEF_SIZE];        //文件在服务器中的长度
};




//获取当地时间
#define LOCALTIME(buf) \
    do{                 \
          time_t rawtime;   \
          struct tm *ptminfo; \
          time(&rawtime);      \
          ptminfo = localtime(&rawtime); \
          sprintf((buf),"%02d-%02d-%02d %02d:%02d:%02d", \
          ptminfo->tm_year + 1900, ptminfo->tm_mon + 1, ptminfo->tm_mday, \
          ptminfo->tm_hour, ptminfo->tm_min, ptminfo->tm_sec);    \
    }while(0)


//获取邮件对应的数据库目录位置
#define GET_USR_DB_PATH(usr_email)   \
    do{                                               \
        string fun_mail = string((usr_email));             \
        if(fun_mail.find(".") != string::npos)              \
        {                                                       \
            fun_mail.erase(fun_mail.find("."),fun_mail.size());  \
            fun_mail = fun_mail + string(".db");         \
            string db_name = string(DEF_USRSQLDB_PATH) + "/" + fun_mail; \
            memset((usr_email),'\0',sizeof((usr_email)));              \
            strcpy((usr_email),db_name.c_str());              \
        }else                                                       \
        {                                                           \
            strcpy((usr_email),"NULL");                                 \
        }                                                           \
    }while(0)


//获取邮件对应的数据库名称
#define GET_USR_DB_NAME(usr_email)   \
    do{                                               \
        string fun_mail = string(usr_email);             \
        if(fun_mail.find(".") != string::npos)              \
        {                                                       \
            fun_mail.erase(fun_mail.find("."),fun_mail.size());  \
            fun_mail = fun_mail + string(".db");         \
            memset((usr_email),'\0',sizeof((usr_email)));              \
            strcpy((usr_email),fun_mail.c_str());              \
        }else                                               \
        {                                                           \
            strcpy((usr_email),"NULL");                                                        \
        }                                                       \
    }while(0)


//通过MD5值获取文件保存路径
#define GET_MD5TOFILE_PATH(md5)  \
    do{                                   \
        sprintf(md5,"%s/%s",DEF_LOADFILE_PATH,md5);\
    }while(0)

