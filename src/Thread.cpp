
/*==========================================================================
*   Copyright (C) 2019 jiang. All rights reserved.
*   
*	Authors:	jiangjunwei(1596102356@qq.com)
*	Date:		2019/08/06 22:09:14
*	Description:
*
==========================================================================*/




#include"Thread.h"



Thread::Thread(int min,int max,int que_max)
{
	thread_max = max;
	thread_min = min;
	thread_alive = 0;
	thread_busy = 0;
	queue_max = que_max;
	queue_cur = 0;
	shut_down = 1;
	killno = 0;
	queue_front = 0;
	queue_rear = 0;
	tids = new pthread_t[max];
	queue_task = new task_t[que_max];

}

Thread::~Thread()
{
	int i = 0;
	
	shut_down = 0;

	while(tids[i] != 0)
	{
		pthread_join(tids[i],NULL);
		i++;
	}

	pthread_join(managertid,NULL);
    
    delete[] tids;
    delete[] queue_task;

}


void* Thread::Thread_Manager_Job(void*arg)
{
	Thread* pool = (Thread*)arg;
	
	int cur;
	int alive;
	int busy;
	int i = 0;
	int add = 0;
	cout<<"Manager Working..."<<endl;

	while(pool->shut_down)
	{
		pthread_mutex_lock(&pool->lock);
		cur = pool->queue_cur;
		alive = pool->thread_alive;
		busy = pool->thread_busy;
		pthread_mutex_unlock(&pool->lock);
		

		//判断扩容条件
		if(((cur >= alive-busy) || (((float)busy/alive*100) >= (float)80)) && ((alive+pool->thread_min) < pool->thread_max))
		{
			for(i=0,add=0;i<pool->thread_max&&add<pool->thread_min;i++)
			{
				if(pool->tids[i] == 0 || pool->If_Thread_Alive(pool->tids[i]))
				{
					pthread_create(&pool->tids[i],&pool->arr,Thread_Custom_Job,(void*)pool);
					pthread_mutex_lock(&pool->lock);
					++pool->thread_alive;
					pthread_mutex_unlock(&pool->lock);
					add++;
				}
			}

			add = 0;

		}


		//判断缩减条件
		if(busy*2 <= (alive-busy) && ((alive-busy - pool->thread_min) >= pool->thread_min))
		{
			pthread_mutex_lock(&pool->lock);
			pool->killno = pool->thread_min;
			pthread_mutex_unlock(&pool->lock);
			cout<<"Manager set killno..."<<endl;
			
			for(i=0;i<pool->thread_min;i++)
			{
				pthread_cond_signal(&pool->empty);
			}
		}

		cout<<"Manager thread tid:"<<(unsigned int)pthread_self()<<"  pthread_pool info:ALIVE:"<<alive<<"  BUSY:"<<busy<<"  LIE:"<<alive-busy<<"  B/A:"<<(float)busy/alive*100<<"  A/ALL"<<(float)alive/pool->thread_max*100<<endl;
		sleep(TIMEOUT);	

	}

	cout<<"Manager shutdown is False exiting..."<<endl;

	pthread_exit(NULL);
    return NULL;
}

void* Thread::Thread_Usr_Job(void*arg)
{
    while(1)
    {
        cout<<"tid:"<<(unsigned int)pthread_self()<<"   正在工作..."<<endl;
        sleep(10);
    }

    return NULL;
}

void* Thread::Thread_Custom_Job(void*arg)
{
    Thread* pool = (Thread*)arg;
	task_t task;
	
	while(pool->shut_down)
	{
		pthread_mutex_lock(&pool->lock);
		while(pool->queue_cur == 0)
		{
			pthread_cond_wait(&pool->empty,&pool->lock);
		}


		if(pool->shut_down != 1 || pool->killno > 0)
		{
			--pool->killno;
			--pool->thread_alive;
			pthread_mutex_unlock(&pool->lock);
			pthread_exit(NULL);
		}


		task.job = pool->queue_task[pool->queue_rear].job;
		task.arg1 = pool->queue_task[pool->queue_rear].arg1;
		task.arg2 = pool->queue_task[pool->queue_rear].arg2;
		task.arg3 = pool->queue_task[pool->queue_rear].arg3;
		++pool->thread_busy;
		--pool->queue_cur;
		pool->queue_rear = (pool->queue_rear+1)%pool->queue_max;
		pthread_mutex_unlock(&pool->lock);
		pthread_cond_signal(&pool->full);
        
        char fun_buf[DEF_SIZE];
        sprintf(fun_buf,"I am Working:%x",(unsigned int)pthread_self());

        cout<<fun_buf<<endl;
	    (*task.job)(task.arg1,task.arg2,task.arg3);

		pthread_mutex_lock(&pool->lock);
		--pool->thread_busy;
		pthread_mutex_unlock(&pool->lock);
	}

	pthread_exit(NULL);

    return NULL;
}

bool Thread::If_Thread_Alive(pthread_t tid)
{
	int m;
	if((m = pthread_kill(tid,0)) > 0)
	{
		if(m == ESRCH)
			return false;
	}
	

    return true;
}

void* Thread::Thread_Task_Add(FUN fun,void* arg1,int arg2,char*arg3)
{
	pthread_mutex_lock(&lock);
	while(queue_cur == queue_max && shut_down == 1)
	{
		pthread_cond_wait(&full,&lock);
	}
	

	if(shut_down != 1)
	{
		pthread_mutex_unlock(&lock);
		pthread_exit(NULL);
	}



    queue_task[queue_front].job = fun;
	queue_task[queue_front].arg1 = arg1;
	queue_task[queue_front].arg2 = arg2;
	queue_task[queue_front].arg3 = arg3;
	
	queue_front = (queue_front + 1)%queue_max;
	++queue_cur;
	pthread_mutex_unlock(&lock);
	pthread_cond_signal(&empty);


    return NULL;
}

bool Thread::Thread_Pool_Create()
{
	if(pthread_mutex_init(&lock,NULL) != 0)
	{
		return false;
	}
	
	if(pthread_cond_init(&full,NULL) != 0)
	{
		return false;
	}

	if(pthread_cond_init(&empty,NULL) != 0)
	{
		return false;
	}

	if(pthread_attr_init(&arr) != 0)
	{
		return false;
	}

	if(pthread_attr_setdetachstate(&arr,PTHREAD_CREATE_DETACHED) != 0)
	{
		return false;
	}


	for(int i=0;i<thread_min;i++)
	{
		pthread_create(&tids[i],&arr,Thread_Custom_Job,this);
		thread_alive++;
	}


	pthread_create(&managertid,&arr,Thread_Manager_Job,this);

    return true;
}
