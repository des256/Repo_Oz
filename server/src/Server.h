// OZ - Robot Puppeteering Server
// (C) Copyrights by Desmond Germans
// 2014 Germans Media Technology & Services

#ifndef _SERVER_H_
#define _SERVER_H_

#include "Task.h"
#include "pool.h"
#include "Servos.h"
#include "SpeechToText.h"
#include "Microphone.h"
#include "Speaker.h"
#include "TextToSpeech.h"
#include "Vision.h"
#include "Camera.h"


class Server : public Task
{
	int lfd;                                // listening socket
	int cfd;                                // client socket (or -1 if no client connected)

	pool<videobuffer> videopool;            // videobuffer pool
	pool<audiobuffer> audiopool;            // audiobuffer pool
	pool<utterancebuffer> utterancepool;    // utterancebuffer pool
	pool<facebuffer> facepool;              // facebuffer pool
	pool<textbuffer> textpool;              // textbuffer pool
	pool<speechbuffer> speechpool;          // speechbuffer pool
	pool<lipsyncbuffer> lipsyncpool;        // lipsyncbuffer pool

	Servos servos;                          // servo control task
	SpeechToText stt;                       // speech-to-text task
	Microphone microphone;                  // microphone input task
	Speaker speaker;                        // speaker output task
	TextToSpeech tts;                       // text-to-speech task
	Vision vision;                          // face detection task
	Camera camera;                          // camera input task

	unsigned char* compressed;              // compressed video frame
	unsigned long int compressed_size;      // compressed video size
	JSAMPROW* image_y,* image_u,* image_v;  // jpeg raw data helper pointers

public:
	Server();
	virtual ~Server();
	virtual void Step();
	virtual void Message(int msg,void* data);

	int Send(unsigned char* buffer,int length);
	int Receive(unsigned char* buffer,int length);
	void SendByte(unsigned char b);
	unsigned char ReceiveByte();
	void SendWord(unsigned short w);
	unsigned short ReceiveWord();
	void SendDword(unsigned int d);
	unsigned int ReceiveDword();
	void SendFloat(float f);
	float ReceiveFloat();
	void SendDouble(double d);
	double ReceiveDouble();
	void SendString(char* s);
	void ReceiveString(char* s,int max);
};


#endif
