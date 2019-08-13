/*==========================================================================
*   Copyright (C) 2019 jiang. All rights reserved.
*   
*	Authors:	jiangjunwei(1596102356@qq.com)
*	Date:		2019/08/05 11:24:43
*	Description:
*
==========================================================================*/

#pragma once
#include"Yun.h"
#include"TCPKernel.h"


typedef void* (*FUN)(void*,int,char*);
typedef struct
{
	FUN job;
    void* arg1;
	int arg2;
    char* arg3;
}task_t;



class Thread
{
public:
    Thread(int max,int min,int que_max);
    ~Thread();

public:
    bool Thread_Pool_Create();
    bool If_Thread_Alive(pthread_t pid);
    void* Thread_Task_Add(FUN,void* arg1,int arg2,char* arg3);
    static void* Thread_Manager_Job(void*arg);
    static void* Thread_Usr_Job(void*arg);
    static void* Thread_Custom_Job(void*arg);

private:
	int thread_max;
	int thread_min;
	int thread_alive;
	int thread_busy;
	int queue_max;
	int queue_cur;
	int shut_down;
	int killno;
	int queue_front;
	int queue_rear;
	pthread_t *tids;
	pthread_t managertid;
	task_t* queue_task;
	pthread_cond_t full;
	pthread_cond_t empty;
	pthread_mutex_t lock;
	pthread_attr_t arr;
};

