// OZ - Robot Puppeteering Server
// (C) Copyrights by Desmond Germans
// 2014 Germans Media Technology & Services

#ifndef _SPEECHTOTEXT_H_
#define _SPEECHTOTEXT_H_

#include "Task.h"
#include "pool.h"


class Server;
class SpeechToText : public Task
{
protected:
	Server& server;                        // server reference
	pool<utterancebuffer>& utterancepool;  // utterancebuffer pool reference
#ifdef USE_GOOGLE_STT
	unsigned char* flacbuffer;             // FLAC output data
	unsigned char* resultbuffer;           // Google's answer buffer
	int fbs;                               // FLAC buffer size
	int rbs;                               // result buffer size
	int sfd;                               // Google server fd
#endif
public:
	SpeechToText(Server& aserver,pool<utterancebuffer>& autterancepool);
	virtual ~SpeechToText();
	virtual void Message(int msg,void* data);
	utterancebuffer* Process(audiobuffer* audio);
#ifdef USE_GOOGLE_STT
	void AppendFLAC(unsigned char* buffer,int length);
	void AppendResult(unsigned char* buffer,int length);
#endif
};


#endif
