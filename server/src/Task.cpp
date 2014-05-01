// OZ - Robot Puppeteering Server
// (C) Copyrights by Desmond Germans
// 2014 Germans Media Technology & Services

#include "Task.h"


struct threadstruct
{
	Task* task;
	volatile bool done;
};


void* start_routine(void* param)
{
	threadstruct* ts = (threadstruct*)param;
	Task* task = ts->task;
	ts->done = true;
	task->Run();
	return 0;
}


Task::Task() : thread(0),rfd(-1),wfd(-1),running(false)
{
	int fd[2];
	fd[0] = -1;
	fd[1] = -1;
	pipe(fd);
	rfd = fd[0];
	wfd = fd[1];
}


Task::~Task()
{
	if(thread)
	{
		struct timespec ts;
		clock_gettime(CLOCK_REALTIME,&ts);
		ts.tv_sec++;
		if(pthread_timedjoin_np(thread,0,&ts))
		{
			// TODO: force-terminate the thread
		}
	}
	if(rfd != -1)
		close(rfd);
	if(wfd != -1)
		close(wfd);
}


void Task::Post(int msg,void* data)
{
	write(wfd,&msg,sizeof(int));
	write(wfd,&data,sizeof(void*));
}


void Task::Start()
{
	threadstruct ts;
	ts.task = this;
	ts.done = false;
	pthread_create((pthread_t*)&thread,0,start_routine,&ts);
	while(!ts.done);
}


void Task::Run()
{
	running = true;
	while(running)
		Step();
}


void Task::Step()
{
	struct pollfd pfd;

	pfd.fd = rfd;
	pfd.events = POLLIN;
	pfd.revents = 0;

	poll(&pfd,1,-1);

	if(pfd.revents & POLLIN)
		FlushMessages();
}


void Task::FlushMessages()
{
	unsigned char buffer[32768],* bptr = buffer;
	int size,packetsize = sizeof(int) + sizeof(void*);

	ioctl(rfd,FIONREAD,&size);
	while(size % packetsize)
		size++;
	int rem = size;
	while(rem)
	{
		int bytes = read(rfd,bptr,size);
		rem -= bytes;
		bptr += bytes;
	}

	int total = bptr - buffer;
	bptr = buffer;
	while(total > 0)
	{
		int msg = *(int*)bptr;
		bptr += sizeof(int);
		void* data = *(void**)bptr;
		bptr += sizeof(void*);
		if(msg == ITM_EXIT)
			Exit();
		else
			Message(msg,data);
		total -= sizeof(int) + sizeof(void*);
	}
}


void Task::Exit()
{
	running = false;
}


void Task::Message(int msg,void* data)
{
	printf("Task::Message: msg = %d, data = %08X\n",msg,(unsigned int)data);
}
