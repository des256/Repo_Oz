// OZ - Robot Puppeteering Server
// (C) Copyrights by Desmond Germans
// 2014 Germans Media Technology & Services

#include "Server.h"


unsigned long long global_start_usec;


int main(int argc,char** argv)
{
	// initialize global clock start
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC,&ts);
	global_start_usec = (ts.tv_sec * 1000000) + (ts.tv_nsec / 1000);

	// start server
	Server server;

	// run server
	server.Run();

	// goodbye
	return 0;
}
