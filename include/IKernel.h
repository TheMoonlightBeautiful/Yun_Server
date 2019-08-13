
/*==========================================================================
*   Copyright (C) 2019 jiang. All rights reserved.
*   
*	Authors:	jiangjunwei(1596102356@qq.com)
*	Date:		2019/08/06 22:09:14
*	Description:
*
==========================================================================*/


#ifndef _IKERNEL_H
#define _IKERNEL_H

#include "INet.h"

class IKernel
{
public:
	IKernel(){}
	virtual ~IKernel(){}

public:
	virtual void Close() = 0;
	virtual void* DealData(int fd,char* szbuf) = 0;

protected:
	INet* m_pNet;

};


#endif
