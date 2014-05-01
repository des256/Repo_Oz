// OZ - Robot Puppeteering Server
// (C) Copyrights by Desmond Germans
// 2014 Germans Media Technology & Services

#ifndef _VISION_H_
#define _VISION_H_

#include "Task.h"
#include "pool.h"


class Server;
class Vision : public Task
{
	Server& server;                  // server reference
	pool<facebuffer>& facepool;      // facebuffer pool reference
	int tfd;                         // timer fd
	videobuffer* current;            // current videobuffer
#ifdef USE_OPENCV_VISION
	cv::CascadeClassifier* cascade;  // Haar cascade classifier
#endif
public:
	Vision(Server& aserver,pool<facebuffer>& afacepool);
	virtual ~Vision();
	virtual void Step();
	virtual void Message(int msg,void* data);
	facebuffer* Process(videobuffer* video);
};


#endif
