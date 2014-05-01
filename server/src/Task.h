// OZ - Robot Puppeteering Server
// (C) Copyrights by Desmond Germans
// 2014 Germans Media Technology & Services

#ifndef _TASK_H_
#define _TASK_H_

#include "base.h"


class Task
{
protected:
	pthread_t thread;       // thread descriptor for the task
	int rfd,wfd;            // message pipe read and write descriptors
	volatile bool running;  // boolean to indicate whether or not the task is active

public:
	Task();
	virtual ~Task();

	void Post(int msg,void* data = 0);
	void Start();
	virtual void Run();
	virtual void Step();
	void FlushMessages();
	virtual void Exit();
	virtual void Message(int msg,void* data);
};


#endif
