// OZ - Robot Puppeteering Server
// (C) Copyrights by Desmond Germans
// 2014 Germans Media Technology & Services

#include "Speaker.h"
#include "Server.h"


Speaker::Speaker(Server& aserver,char* name) : Task(),server(aserver),pcm(0)
{
	if(snd_pcm_open(&pcm,name,SND_PCM_STREAM_PLAYBACK,0) < 0)
	{
		printf("Unable to open speaker PCM device (%s), so no audio available\n",name);
		return;
	}

	// specify hardware parameters
	snd_pcm_hw_params_t* params = 0;
	snd_pcm_hw_params_alloca(&params);
	if(!params)
	{
		printf("Cannot allocate speaker hardware parameters, so no audio available\n");
		snd_pcm_close(pcm);
		pcm = 0;
		return;
	}
	if(snd_pcm_hw_params_any(pcm,params) < 0)
	{
		printf("Cannot initialize speaker hardware parameters, so no audio available\n");
		snd_pcm_close(pcm);
		pcm = 0;
		return;
	}
	if(snd_pcm_hw_params_set_access(pcm,params,SND_PCM_ACCESS_RW_INTERLEAVED) < 0)
	{
		printf("Cannot set speaker access mode to interleaved, so no audio available\n");
		snd_pcm_close(pcm);
		pcm = 0;
		return;
	}
	if(snd_pcm_hw_params_set_format(pcm,params,SND_PCM_FORMAT_S16_LE) < 0)
	{
		printf("Cannot set speaker sample format to signed 16-bit little-endian, so no audio available\n");
		snd_pcm_close(pcm);
		pcm = 0;
		return;
	}
	if(snd_pcm_hw_params_set_channels(pcm,params,1) < 0)
	{
		printf("Cannot set number of speaker channels to mono, so no audio available\n");
		snd_pcm_close(pcm);
		pcm = 0;
		return;
	}
	if(snd_pcm_hw_params_set_rate(pcm,params,22050,0) < 0)
	{
		printf("Cannot set speaker sampling rate to 22050Hz, so no audio available\n");
		snd_pcm_close(pcm);
		pcm = 0;
		return;
	}
	if(snd_pcm_hw_params(pcm,params) < 0)
	{
		printf("Cannot set speaker hardware parameters, so no audio available\n");
		snd_pcm_close(pcm);
		pcm = 0;
		return;
	}

	printf("Initialized speaker\n");
}


Speaker::~Speaker()
{
	if(pcm)
	{
		snd_pcm_drain(pcm);
		snd_pcm_close(pcm);
	}
}


void Speaker::Message(int msg,void* data)
{
	switch(msg)
	{
		case ITM_SPEECH:
			{
				speechbuffer* speech = (speechbuffer*)data;
				if(snd_pcm_writei(pcm,speech->frame,speech->frames) == -EPIPE)
				{
					snd_pcm_prepare(pcm);
					snd_pcm_writei(pcm,speech->frame,speech->frames);
				}
				speech->release();
			}
			break;
	}
}
