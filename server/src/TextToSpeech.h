// OZ - Robot Puppeteering Server
// (C) Copyrights by Desmond Germans
// 2014 Germans Media Technology & Services

#ifndef _TEXTTOSPEECH_H_
#define _TEXTTOSPEECH_H_

#include "Task.h"
#include "pool.h"


// maximum number of visemes processed by Acapela mark callback
#define MAX_VISEMES 1024

// maximum number of words processed by Acapela mark callback
#define MAX_WORDS 1024


class Server;
class Speaker;
class Servos;
class Microphone;
class TextToSpeech : public Task
{
	Server& server;                    // server reference
	Speaker& speaker;                  // speaker reference
	Servos& servos;                    // servos reference
	Microphone& microphone;            // microphone reference
	pool<speechbuffer>& speechpool;    // speechbuffer pool reference
	pool<lipsyncbuffer>& lipsyncpool;  // lipsyncbuffer pool reference
#ifdef USE_ACAPELA_TTS
	friend BB_S32 MarkCallback(BABILE_EventCallBackStruct* info);
	BABILE_Obj* bab;                       // Acapela instance object
	BB_DbLs* voice;                        // Acapela voice descriptor
	int visemes;                           // number of visemes
	unsigned long long vts[MAX_VISEMES];   // viseme timestamps
	int vid[MAX_VISEMES];                  // viseme IDs
	int words;                             // number of words
	unsigned long long wts[MAX_WORDS];     // word timestamps
	int wcp[MAX_WORDS];                    // word character positions
#endif
#ifdef USE_IVONA_TTS
	tts_instance* instance;                // Ivona instance object
	tts_voice* voice;                      // Ivona voice descriptor
#endif

public:
	TextToSpeech(Server& aserver,Speaker& aspeaker,Servos& aservos,Microphone& amicrophone,pool<speechbuffer>& aspeechpool,pool<lipsyncbuffer>& alipsyncpool);
	virtual ~TextToSpeech();

	// handlers
	virtual void Message(int msg,void* data);
	speechbuffer* Process(textbuffer* text,lipsyncbuffer*& lipsync);
};


#endif
