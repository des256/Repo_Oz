// OZ - Robot Puppeteering Server
// (C) Copyrights by Desmond Germans
// 2014 Germans Media Technology & Services

#ifndef _SPEAKER_H_
#define _SPEAKER_H_

#include "Task.h"


class Server;
class Speaker : public Task
{
	Server& server;          // owning server reference
	snd_pcm_t* pcm;          // PCM device

public:
	Speaker(Server& aserver,char* name);
	virtual ~Speaker();
	virtual void Message(int msg,void* data);
};


#endif
