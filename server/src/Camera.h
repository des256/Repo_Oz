// OZ - Robot Puppeteering Server
// (C) Copyrights by Desmond Germans
// 2014 Germans Media Technology & Services

#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "Task.h"
#include "pool.h"


// maximum number of video buffers to ask from video4linux
#define MAX_BUFFERS 5


enum
{
	VIDEO_UNKNOWN = 0,  // video buffer format unknown
	VIDEO_YUV420,       // typical YUV 4:2:0
	VIDEO_YUYV,         // YUYV format, used sometimes
	VIDEO_BA81          // Robokind robots use this
};


class Server;
class Streamer;
class Vision;
class Camera : public Task
{
	Server& server;                // owning server reference
	pool<videobuffer>& videopool;  // videobuffer pool
	int cfd;                       // camera device
	int type;                      // video type (one of VIDEO_*)
	struct Buffer
	{
		unsigned char* mapped;
		int length;
	} buffer[MAX_BUFFERS];         // memory-mapped framebuffers

public:
	Camera(Server& aserver,pool<videobuffer>& avideopool,char* name);
	virtual ~Camera();
	virtual void Run();
	virtual void Step();
};


#endif
