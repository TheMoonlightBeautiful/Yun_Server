#ifndef _INET_H
#define _INET_H

#include "Packdef.h"
#include <list>


class INet
{
public:
	INet(){}
	virtual ~INet(){}

public:
	virtual bool InitNetWork() = 0;
	virtual void UnInitNetWork() = 0;
	virtual bool SendData(int fd,char* szbuf, int nlen) = 0;
	virtual bool DealData(int fd,char* szbuf) = 0;

};


#endif
