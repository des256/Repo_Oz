// OZ - Robot Puppeteering Server
// (C) Copyrights by Desmond Germans
// 2014 Germans Media Technology & Services

#ifndef _MICROPHONE_H_
#define _MICROPHONE_H_

#include "Task.h"
#include "pool.h"


class Server;
class SpeechToText;
class Microphone : public Task
{
	Server& server;                // reference to owning server
	SpeechToText& stt;             // speech-to-text task to process the incoming samples
	pool<audiobuffer>& audiopool;  // audiobuffer pool
	snd_pcm_t* pcm;                // PCM device
	int ratemul;                   // sampling rate multiplier (1 for 22050Hz, 2 for 44100Hz)
	int channels;                  // number of channels (1 for mono, 2 for stereo)
	int mfd;                       // microphone fd
	short* micbuffer;              // buffer with currently sampled audio
	short* prereadbuffer;          // buffer with preread audio
	int prr,prw;                   // prereadbuffer read and write pointers
	short* extractbuffer;          // extracted audio buffer
	int ebs;                       // size of extracted audio buffer
	int sc;                        // number of non-loud samples
	bool recording;                // true if recording a sample
	unsigned long long usec;       // current time timestamp, measured from received samples
	bool muted;                    // microphone is muted by text-to-speech

public:
	Microphone(Server& aserver,SpeechToText& astt,pool<audiobuffer>& aaudiopool,char* name);
	virtual ~Microphone();
	virtual void Step();
	virtual void Message(int msg,void* data);
};


#endif
