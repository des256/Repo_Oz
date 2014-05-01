// OZ - Robot Puppeteering Server
// (C) Copyrights by Desmond Germans
// 2014 Germans Media Technology & Services

#include "Vision.h"
#include "Server.h"


#define VISION_MSEC 5000


Vision::Vision(Server& aserver,pool<facebuffer>& afacepool) : Task(),server(aserver),facepool(afacepool),tfd(-1),current(0)
{
	// start timer
	tfd = timerfd_create(CLOCK_REALTIME,0);
	struct itimerspec its;
	its.it_interval.tv_sec = VISION_MSEC / 1000;
	its.it_interval.tv_nsec = (VISION_MSEC % 1000) * 1000000;
	its.it_value.tv_sec = VISION_MSEC / 1000;
	its.it_value.tv_nsec = (VISION_MSEC % 1000) * 1000000;
	timerfd_settime(tfd,0,&its,0);

#ifdef USE_OPENCV_VISION
	cascade = new cv::CascadeClassifier(FACE_CASCADE_FILE);
	printf("Initialized OpenCV face detection\n");
#else
	printf("No face detection available\n");
#endif
}


Vision::~Vision()
{
#ifdef USE_OPENCV_VISION
	delete cascade;
#endif
	if(current)
	{
		current->release();
		current = 0;
	}
}


void Vision::Step()
{
	// prepare poll structures
	struct pollfd pfd[2];

	pfd[0].fd = rfd;
	pfd[0].events = POLLIN;
	pfd[0].revents = 0;

	pfd[1].fd = tfd;
	pfd[1].events = POLLIN;
	pfd[1].revents = 0;

	poll(pfd,2,-1);

	if(pfd[0].revents & POLLIN)
		FlushMessages();

	if(pfd[1].revents & POLLIN)
	{
		unsigned char buffer[8];
		read(tfd,buffer,8);

		if(current)
		{
			//printf("Vision: processing current videobuffer\n");
			facebuffer* result = Process(current);
			if(result)
			{
				//printf("Vision: posting facebuffer to server\n");
				server.Post(ITM_FACES,result);
			}
		}
	}
}


void Vision::Message(int msg,void* data)
{
	switch(msg)
	{
		case ITM_VIDEO:
			{
				//printf("Vision: switching to new videobuffer\n");
				videobuffer* video = (videobuffer*)data;
				if(current)
					current->release();
				current = video;
			}
			break;
	}
}


facebuffer* Vision::Process(videobuffer* video)
{
#ifdef USE_OPENCV_VISION
	cv::Mat image(VIDEO_YSIZE,VIDEO_XSIZE,CV_8UC1,(void*)video);
	std::vector<cv::Rect> faces;
	cascade->detectMultiScale(image,faces);
	if(faces.size() != 0)
	{
		facebuffer* result = facepool.get();
		result->faces = faces.size();
		for(int i = 0; i < result->faces; i++)
		{
			strcpy(result->face[i].identity,"unknown");
			result->face[i].rect = intr(int2(faces[i].x,faces[i].y),int2(faces[i].width,faces[i].height));
		}
		return result;
	}
#endif
	return 0;
}
