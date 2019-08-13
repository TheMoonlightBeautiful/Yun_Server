
/*==========================================================================
*   Copyright (C) 2019 jiang. All rights reserved.
*   
*	Authors:	jiangjunwei(1596102356@qq.com)
*	Date:		2019/08/06 22:09:14
*	Description:
*
==========================================================================*/


#include "TCPKernel.h"
#include "Thread.h"

extern Thread* pthread;

TCPKernel::TCPKernel()
{
    flgs = false;
    p_sql_kernel = NULL;
    p_tcp_net = NULL;
}

TCPKernel::~TCPKernel()
{
    if(p_sql_kernel != NULL)
        delete p_sql_kernel;
    if(p_tcp_net != NULL){
        TCPNet::Del_TCPNet();
        p_tcp_net = NULL;
    }
        

}

ProtocolMap m_ProtocolEntries[] = 
{
	{_DEF_REGISTER_RQ,&TCPKernel::RegisterRq},
	{_DEF_LOGIN_RQ,&TCPKernel::LoginRq},
    {_DEF_GETSQLDB_RQ,&TCPKernel::GetSqlDBRq},
    {_DEF_INSERINTOSQL_RQ,&TCPKernel::InSerFileRq},
    {_DEF_DELETEFILEINFOSQL_RQ,&TCPKernel::DeleteFileRq},
    {_DEF_FILE_HEADINFO_RQ,&TCPKernel::UpLoadFileHeadInfoRq},
    {_DEF_UPLOAD_RQ,&TCPKernel::UpLoadFileInfoRq},
    {_DEF_DOWN_RQ,&TCPKernel::DownFileInfoRq},
    {_DEF_UPLOAD_END,&TCPKernel::UpLoadFileInfoRs},
	{0,0}
};

void* TCPKernel::DownFileInfoRq(int sockfd,char* szbuf)                   //接收用户下载文件请求
{
    STRU_DOWNFILE_RQ* ssr = (STRU_DOWNFILE_RQ*)szbuf;          //下载文件
    STRU_DOWNFILE res;
    res.m_nType = _DEF_DOWN_RS;
    res.m_fd = ssr->m_fd;
    strcpy(res.m_szName,ssr->m_szName);
    res.m_readLen = ssr->m_pos;

    FILE* fun_fd = fopen(ssr->m_szName,"rb");
    if(fun_fd != NULL){
        fseek(fun_fd,ssr->m_pos,SEEK_SET);
        res.m_nLen = fread(res.m_szContent,sizeof(unsigned char),sizeof(res.m_szContent),fun_fd);
        fclose(fun_fd);
    }else{
        res.m_nLen = 0;
    }
    

	p_tcp_net->SendData(sockfd,(char*)&res,sizeof(res));

    return (void*)true;

}

void* TCPKernel::UpLoadFileInfoRs(int sockfd,char* szbuf)            //接收用户上传完成回复
{
	STRU_TOSQL_UP_FILEINFO* fifo = (STRU_TOSQL_UP_FILEINFO*)szbuf;   //用于添加用户上传完成文件信息
   
	FILE* fun_fd = NULL;
    char file_path[DEF_SIZE];
	int filelen = 0;
    //strcpy(file_path,fifo->m_szMD5);
    //GET_MD5TOFILE_PATH(file_path);
    sprintf(file_path,"%s/%s",DEF_LOADFILE_PATH,fifo->m_szMD5);
	fun_fd = fopen(file_path,"r");   //通过文件MD5值构建文件名,判断是否上传成功
    
    if(fun_fd != NULL)
    {
    	fseek(fun_fd,0,SEEK_END);
	    filelen = ftell(fun_fd);
    }

	int i = 10;
	while ((i > 0) && (filelen < atoi(fifo->m_szFileSize))) {	
		fclose(fun_fd);
		fun_fd = NULL;
		filelen = 0;
		sleep(1);
		i--;
		fun_fd = fopen(file_path, "r");
        if(fun_fd != NULL)
        {
		    fseek(fun_fd, 0, SEEK_END);
		    filelen = ftell(fun_fd);
        }
	}

    if(fun_fd != NULL)
	    fclose(fun_fd);

    STRU_TOSQL_UP_FILEINFO sql_fifo = *fifo;
    sql_fifo.m_nType = _DEF_INSERINTOSQL_RQ;

    //上传失败,返回插入文件失败
    if (filelen < atoi(fifo->m_szFileSize))  
     {
        strcpy(sql_fifo.m_szFileType,"FAILED");
     }

    InSerFileRq(sockfd,(char*)&sql_fifo);
	
    cout<<"DATA SEND END...................."<<endl;

    return (void*)NULL;
}



void* TCPKernel::UpLoadFileInfoRq(int sockfd,char* szbuf)      //获取用户上传的文件块进行保存用户上传文件
{
    STRU_UPLOADFILE* ssr = (STRU_UPLOADFILE*)szbuf;
    STRU_UPLOADFILE_RQ  res;//用于回复,获取其他文件资源
    res.m_nType = _DEF_UPLOAD_RS; 
    res.m_fd = ssr->m_fd;
    strcpy(res.m_szName,ssr->m_szName);

	FILE* fd = NULL;
	fd = fopen(ssr->m_szName,"ab+");
    if(fd == NULL)
        return (void*)false;
	int w_len = 0;

	w_len = fwrite(ssr->m_szContent,sizeof(unsigned char),ssr->m_nLen, fd);
	fclose(fd);
    res.m_pos = ssr->m_readLen + w_len;
	p_tcp_net->SendData(sockfd,(char*)&res,sizeof(res));
    return (void*)true;

}


void* TCPKernel::RegisterRq(int sockfd,char* szbuf)
{
	STRU_REGISTER_RQ *psrr = (STRU_REGISTER_RQ*)szbuf;
	STRU_REGISTER_RQ sqlsz = *psrr;
	STRU_REGISTER_RS ssr;
	ssr.m_nType = _DEF_REGISTER_RS;
    
    //数据库添加信息
    if(!(bool)p_sql_kernel->DealData((char*)&sqlsz))
        ssr.m_szResult = register_fail;
    else
        ssr.m_szResult = register_success;
        	
	//发送回复
	p_tcp_net->SendData(sockfd,(char*)&ssr,sizeof(ssr));

	return (void*)true;
}

void* TCPKernel::LoginRq(int sockfd,char* szbuf)
{
	STRU_LOGIN_RQ *pslr = (STRU_LOGIN_RQ*)szbuf;
	STRU_LOGIN_RQ sqlsz = *pslr;
	//将用户信息取出来,比较
	STRU_LOGIN_RS slr;
	slr.m_nType = _DEF_LOGIN_RS;

    //查询数据库
    bool sql_sult = (bool)p_sql_kernel->DealData((char*)&sqlsz);
    if(sql_sult == false)
        slr.m_szResult = login_fail;
    else
        slr.m_szResult = login_success; 

    cout<<"LOG_EMAIL:"<<sqlsz.m_szEmail<<endl;
    char fun_dbname[DEF_SIZE];
    strcpy(fun_dbname,sqlsz.m_szEmail);
    GET_USR_DB_NAME(fun_dbname);  
    cout<<"LOGIN_EMAILE_DBNAME:"<<fun_dbname<<endl;

    memset(slr.m_szDBName,'\0',sizeof(slr.m_szDBName));
    strcpy(slr.m_szDBName,fun_dbname);
	//发送回复
	p_tcp_net->SendData(sockfd,(char*)&slr,sizeof(slr));
	return (void*)true;
}


void* TCPKernel::GetSqlDBRq(int sockfd,char* szbuf)
{   

    cout<<"GET SQL DB RQ IS WORKING...."<<endl;
    STRU_GETSQLDB_RQ* ssr = (STRU_GETSQLDB_RQ*)szbuf;
    STRU_GETSQLDB_RS  hsr;  //回复
    hsr.m_nType = _DEF_GETSQLDB_RS; 
    char fun_dbpath[DEF_SIZE];
    char fun_dbname[DEF_SIZE];
    strcpy(fun_dbpath,ssr->m_szEmail);
    strcpy(fun_dbname,ssr->m_szEmail);
    
    GET_USR_DB_PATH(fun_dbpath);            //获取邮箱对应的数据哭目录
    GET_USR_DB_NAME(fun_dbname);            //获取邮箱对应的数据哭目录
    
    cout<<"FUN_DBPATH:"<<fun_dbpath<<endl;
    cout<<"FUN_DBNAME:"<<fun_dbname<<endl;

    strcpy(hsr.m_szDBName,fun_dbname);
    strcpy(hsr.m_szDBPath,fun_dbpath);

    FILE* fun_fd = NULL;
    fun_fd = fopen(fun_dbpath,"r");
    
    if(fun_fd == NULL)
    {
        hsr.m_nlen = 0;         //回复数据库的长度
	    p_tcp_net->SendData(sockfd,(char*)&hsr,sizeof(hsr));
        return (void*)false;
    }
    
	int flen;
	fseek(fun_fd,0,SEEK_END);
	flen = ftell(fun_fd);       //文件大小
    fclose(fun_fd);
    hsr.m_nlen = flen;  //数据库文件的总长度,用于给客户端校验
	p_tcp_net->SendData(sockfd,(char*)&hsr,sizeof(hsr));
    cout<<"GET SQL DB RQ IS END...."<<endl;

    return (void*)true;
}


void* TCPKernel::InSerFileRq(int sockfd,char*szbuf)
{
	STRU_TOSQL_UP_FILEINFO* fifo = (STRU_TOSQL_UP_FILEINFO*)szbuf;
	STRU_TOSQL_UP_FILEINFO sql_fifo = *fifo;
    STRU_TOSQL_UP_FILEINFO_RS cli_fifo;
    cli_fifo.m_nType = _DEF_INSERINTOSQL_RS;        //用于向用户发送回复
    bool ret;
    if(strcmp(fifo->m_szFileType,"FALIED") != 0)
    {
	    ret = (bool)p_sql_kernel->DealData((char*)&sql_fifo);
        if(ret)
            strcpy(cli_fifo.m_szResult,"1");
        else
            strcpy(cli_fifo.m_szResult,"0");
    }
    else
        strcpy(cli_fifo.m_szResult,"0");

    strcpy(cli_fifo.m_szEmail,fifo->m_szEmail);
    strcpy(cli_fifo.m_szFileName,fifo->m_szFileName);
    strcpy(cli_fifo.m_szFileType,fifo->m_szFileType);
    strcpy(cli_fifo.m_szFileSize,fifo->m_szFileSize);
    strcpy(cli_fifo.m_szFileUpLoadTime,fifo->m_szFileUpLoadTime);
    strcpy(cli_fifo.m_szMD5,fifo->m_szMD5);
    strcpy(cli_fifo.m_szFilePath,fifo->m_szFilePath);
 	
	p_tcp_net->SendData(sockfd,(char*)&cli_fifo,sizeof(cli_fifo));
    return (void*)ret;
	
}


void* TCPKernel::DeleteFileRq(int sockfd,char*szbuf)
{

    STRU_DELEETEFILEINFO_RQ* fifo = (STRU_DELEETEFILEINFO_RQ*)szbuf;
    STRU_DELEETEFILEINFO_RQ  sql_fifo = *fifo;
    STRU_DELEETEFILEINFO_RS  cli_fifo;
    cli_fifo.m_nType = _DEF_DELETEFILEINFOSQL_RS;	

	bool ret = (bool)p_sql_kernel->DealData((char*)&sql_fifo);

    strcpy(cli_fifo.m_szEmail,fifo->m_szEmail);
    strcpy(cli_fifo.m_szFileName,fifo->m_szFileName);
    strcpy(cli_fifo.m_szFilePath,fifo->m_szFilePath);
    strcpy(cli_fifo.m_szFatherFilePath,fifo->m_szFatherFilePath);

    if(ret)
        cli_fifo.m_szResult = '1';
    else
        cli_fifo.m_szResult = '0';
     	
	p_tcp_net->SendData(sockfd,(char*)&cli_fifo,sizeof(cli_fifo));
    return (void*)ret;
	
}


void* TCPKernel::UpLoadFileHeadInfoRq(int sockfd,char* szbuf)             //用户上传文件头信息
{
	STRU_TOSQL_UP_FILEINFO* fifo = (STRU_TOSQL_UP_FILEINFO*)szbuf;
    cout<<fifo->m_nType<<endl;
	STRU_TOSQL_UP_FILEINFO sql_fifo = *fifo;
   
    STRU_TOSQL_UP_FILEINFO_RS  res;    //文件头回复
    res.m_nType = _DEF_FILE_HEADINFO_RS;
    strcpy(res.m_szEmail,fifo->m_szEmail);
    strcpy(res.m_szFileName,fifo->m_szFileName);
    strcpy(res.m_szFileType,fifo->m_szFileType);
    strcpy(res.m_szFileSize,fifo->m_szFileSize);
    strcpy(res.m_szFileUpLoadTime,fifo->m_szFileUpLoadTime);
    strcpy(res.m_szMD5,fifo->m_szMD5);

    //发送给数据库,判断文件信息(存在,或者还未上传完成)
    SQL_FILEINFO_RES  *sql_res = (SQL_FILEINFO_RES*)p_sql_kernel->DealData((char*)&sql_fifo);

    if(!strcmp(sql_res->file_path,"NULL"))
    {
       //服务器无法保存该文件
       res.m_id = -1;
       strcpy( res.m_szResult,"0");
       strcpy(res.m_szFilePath,sql_res->file_path);      //该文件在数据库中应该保存位置,让用户上传文件使用
    }else
    {
        res.m_id = sql_res->file_id;
        strcpy(res.m_szResult,sql_res->file_len);
        strcpy(res.m_szFilePath,sql_res->file_path);      //该文件在数据库中应该保存位置,让用户上传文件使用
    }

	p_tcp_net->SendData(sockfd,(char*)&res,sizeof(res));

    delete sql_res;
    sql_res = NULL;
    return (void*)true;
}





bool TCPKernel::Open(char* host,char* usr,char* password,char* DBname)
{
    p_sql_kernel = new SQLKernel;
    p_tcp_net = TCPNet::Get_TCPNet();

    if(p_sql_kernel == NULL || p_tcp_net == NULL)
        return false;

    if(!p_sql_kernel->Open(host,usr,password,DBname))
        return false;

	return true;
}

void TCPKernel::Close()
{
    if(p_tcp_net != NULL){
        TCPNet::Del_TCPNet();
        p_tcp_net = NULL;
    }

}

void* TCPKernel::DealData(void* ptc,int sockfd,char* szbuf)
{
	//处理协议 
    TCPKernel* ptcp = (TCPKernel*)ptc;
	char *ptype = (char*)szbuf;
	int i = 0;
	while(1)
	{
		if(m_ProtocolEntries[i].m_nType == *ptype)
		{
			return (void*)(ptcp->*m_ProtocolEntries[i].m_pfun)(sockfd,szbuf);
			break;
		}
		else if(m_ProtocolEntries[i].m_nType == 0 && m_ProtocolEntries[i].m_pfun ==0)
			break;

		i++;
	}

	return (void*)true;
}
