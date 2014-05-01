// OZ - Robot Puppeteering Server
// (C) Copyrights by Desmond Germans
// 2014 Germans Media Technology & Services

#include "Microphone.h"
#include "Server.h"
#include "SpeechToText.h"

// number of frames to have read before an interrupt is triggered on the microphone fd
#define MAX_TRIGGER_FRAMES 4096

// size of the micrphone buffer that receives the initial samples directly from the microphone
#define MICBUFFER_SIZE (10 * 22050)

// size of the preread ringbuffer that temporarily stores everything for read-back capability
#define PREREADBUFFER_SIZE (10 * 22050)

// size of the extract buffer that contains the sample being recorded
#define EXTRACTBUFFER_SIZE (60 * 22050)

// level (0..32767) to switch from idle to recording
#define IDLE_SILENCE 24000

// level (0..32768) below which the signal is noted as silent
#define RECORDING_SILENCE 8000

// number of samples to prepend each recording from the preread buffer
#define PREREAD_LENGTH 2000

// number of silent samples needed before stopping recording
#define SILENCE_HISTERESIS 8000


Microphone::Microphone(Server& aserver,SpeechToText& astt,pool<audiobuffer>& aaudiopool,char* name) : Task(),server(aserver),stt(astt),audiopool(aaudiopool),pcm(0),ratemul(1),mfd(-1),micbuffer(0),prereadbuffer(0),prr(0),prw(0),extractbuffer(0),ebs(0),sc(0),recording(false),usec(0)
{
	if(snd_pcm_open(&pcm,name,SND_PCM_STREAM_CAPTURE,SND_PCM_NONBLOCK) < 0)
	{
		printf("Unable to open microphone PCM device (%s), so no audio available\n",name);
		return;
	}

	snd_pcm_hw_params_t* hwparams = 0;
	snd_pcm_hw_params_alloca(&hwparams);
	if(!hwparams)
	{
		printf("Cannot allocate microphone hardware parameters, so no audio available\n");
		return;
	}
	if(snd_pcm_hw_params_any(pcm,hwparams) < 0)
	{
		printf("Cannot initialize microphone hardware parameters, so no audio available\n");
		return;
	}
	if(snd_pcm_hw_params_set_access(pcm,hwparams,SND_PCM_ACCESS_RW_INTERLEAVED) < 0)
	{
		printf("Cannot set microphone access mode to interleaved, so no audio available\n");
		return;
	}
	if(snd_pcm_hw_params_set_format(pcm,hwparams,SND_PCM_FORMAT_S16_LE) < 0)
	{
		printf("Cannot set microphone sample format to signed 16-bit little-endian, so no audio available\n");
		return;
	}
	if(snd_pcm_hw_params_set_channels(pcm,hwparams,1) < 0)
	{
		printf("Cannot set number of microphone channels, so no audio available\n");
		return;
	}
	if(snd_pcm_hw_params_set_rate(pcm,hwparams,22050,0) < 0)
	{
		printf("Cannot set microphone sampling rate, so no audio available\n");
		return;
	}
	if(snd_pcm_hw_params(pcm,hwparams) < 0)
	{
		printf("Cannot set microphone hardware parameters, so no audio available\n");
		return;
	}
	if(snd_pcm_hw_params_current(pcm,hwparams) < 0)
	{
		printf("Cannot get current microphone hardware parameters, so no audio available\n");
		return;
	}
	unsigned int val;
	if(snd_pcm_hw_params_get_channels(hwparams,&val) < 0)
	{
		printf("Cannot verify current number of microphone channels, assuming mono\n");
		channels = 1;
	}
	else
		channels = val;
	unsigned int rate = 22050;
	int dir;
	if(snd_pcm_hw_params_get_rate(hwparams,&val,&dir) < 0)
		printf("Cannot verify current microphone sampling rate, assuming 22050Hz\n");
	else
		rate = val;
	if(rate == 44100)
		ratemul = 2;
	else if(rate == 22050)
		ratemul = 1;
	else
	{
		printf("Unsupported microphone sampling rate (%dHz), so no audio available\n",rate);
		return;
	}
	snd_pcm_sw_params_t* swparams = 0;
	snd_pcm_sw_params_alloca(&swparams);
	if(!swparams)
	{
		printf("Cannot allocate microphone software parameters, so no audio available\n");
		return;
	}
	if(snd_pcm_sw_params_current(pcm,swparams) < 0)
	{
		printf("Cannot initialize microphone software parameters, so no audio available\n");
		return;
	}
	if(snd_pcm_sw_params_set_avail_min(pcm,swparams,MAX_TRIGGER_FRAMES * ratemul) < 0)
	{
		printf("Cannot set microphone minimum available count, so no audio available\n");
		return;
	}
	if(snd_pcm_sw_params_set_start_threshold(pcm,swparams,0) < 0)
	{
		printf("Cannot set microphone start mode, so no audio available\n");
		return;
	}
	if(snd_pcm_sw_params(pcm,swparams) < 0)
	{
		printf("Cannot set microphone software parameters, so no audio available\n");
		return;
	}
	struct pollfd pfd;
	if(snd_pcm_poll_descriptors(pcm,&pfd,1) < 0)
	{
		printf("Microphone::Microphone: cannot get poll descriptors\n");
		return;
	}
	mfd = pfd.fd;

	micbuffer = new short[MICBUFFER_SIZE];
	prereadbuffer = new short[PREREADBUFFER_SIZE];
	extractbuffer = new short[EXTRACTBUFFER_SIZE];

	printf("Initialized microphone\n");
}


Microphone::~Microphone()
{
	delete[] extractbuffer;
	delete[] prereadbuffer;
	delete[] micbuffer;
	if(pcm)
		snd_pcm_close(pcm);
}


void Microphone::Step()
{
	// prepare poll structures
	struct pollfd pfd[2];

	// message pipe
	pfd[0].fd = rfd;
	pfd[0].events = POLLIN;
	pfd[0].revents = 0;

	// microphone
	pfd[1].fd = mfd;
	pfd[1].events = POLLIN;
	pfd[1].revents = 0;

	// do the poll
	poll(pfd,2,-1);

	// check for incoming messages on the pipe
	if(pfd[0].revents & POLLIN)
		FlushMessages();

	// check for incoming sample data
	if(pfd[1].revents & POLLIN)
	{
        // read from the mic
		int count = snd_pcm_readi(pcm,micbuffer,MICBUFFER_SIZE);

		// process incoming samples
		for(int i = 0; i < count / (ratemul * channels); i++)
		{
			// get mono sample value at 22050Hz
			int value;
			if(ratemul == 2)
				if(channels == 2)
					value = (micbuffer[i * 4] + micbuffer[i * 4 + 1] + micbuffer[i * 4 + 2] + micbuffer[i * 4 + 3]) / 4;
				else
					value = (micbuffer[i * 2] + micbuffer[i * 2 + 1]) / 2;
			else
				if(channels == 2)
					value = (micbuffer[i * 2] + micbuffer[i * 2 + 1]) / 2;
				else
					value = micbuffer[i];

			// put in prereadbuffer
			prereadbuffer[prw++] = value;
			if(prw == PREREADBUFFER_SIZE)
				prw = 0;

			if(!muted)
			{
				if(recording)
				{
					// we're recording, so append the current sample
					if(ebs < EXTRACTBUFFER_SIZE)
						extractbuffer[ebs++] = value;
					else
					{
						// sample is full, so force speech-to-text
						audiobuffer* audio = audiopool.get();
						audio->frames = ebs;
						memcpy(audio->frame,extractbuffer,ebs * sizeof(short));
						printf("Microphone: posting audiobuffer to stt\n");
						stt.Post(ITM_AUDIO,audio);
						recording = false;
						ebs = 0;
						sc = 0;
					}

					if((value > RECORDING_SILENCE) || (value < -RECORDING_SILENCE))
						sc = 0;  // not silent, so reset silent sample counter
					else
					{
						// otherwise, count number of silent samples
						sc++;

						// if waited enough, stop recording
						if(sc >= SILENCE_HISTERESIS)
						{
							// enough non-loud samples to cut off and analyze
							audiobuffer* audio = audiopool.get();
							audio->frames = ebs;
							memcpy(audio->frame,extractbuffer,ebs * sizeof(short));
							stt.Post(ITM_AUDIO,audio);
							recording = false;
							ebs = 0;
							sc = 0;
						}
					}
				}
				else
				{
					if((value > IDLE_SILENCE) || (value < -IDLE_SILENCE))
					{
						// record relative time of start
						struct timespec ts;
						clock_gettime(CLOCK_MONOTONIC,&ts);
						usec = ((ts.tv_sec * 1000000) + (ts.tv_nsec / 1000)) - ((unsigned long long)PREREAD_LENGTH * 1000000UL) / 22050;

						// start with empty sample
						memset(extractbuffer,0,EXTRACTBUFFER_SIZE * sizeof(short));

						// in silent mode, the loudness threshold was passed
						recording = true;
						ebs = 0;
						sc = 0;

						//start with PREREAD_LENGTH samples from the prereadbuffer
						int arp = prw - PREREAD_LENGTH;
						if(arp < 0)
							arp += PREREADBUFFER_SIZE;
						for(int k = 0; k < PREREAD_LENGTH; k++)
						{
							extractbuffer[ebs++] = prereadbuffer[arp++];
							if(arp >= PREREADBUFFER_SIZE)
								arp = 0;
						}

						// and append the current sample
						extractbuffer[ebs++] = value;
					}
				}
			}
		}
	}
}


void Microphone::Message(int msg,void* data)
{
	switch(msg)
	{
		case ITM_MUTE:
			muted = true;
			break;

		case ITM_UNMUTE:
			muted = false;
			break;
	}
}
