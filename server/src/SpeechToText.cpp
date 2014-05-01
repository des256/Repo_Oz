// OZ - Robot Puppeteering Server
// (C) Copyrights by Desmond Germans
// 2014 Germans Media Technology & Services

#include "SpeechToText.h"
#include "Server.h"


#ifdef USE_GOOGLE_STT
// size of the FLAC output buffer
#define FLACBUFFER_SIZE (1024 * 1024)

// size of the result buffer
#define RESULTBUFFER_SIZE (1024 * 1024)

// size of the chunk used for conversion to FLAC
#define CHUNK_SIZE 16384

// the Google URLs for various languages
#define GOOGLE_URL_EN "https://www.google.com/speech-api/v1/recognize?xjerr=1&client=chromium&lang=en-US"
#define GOOGLE_URL_NL "https://www.google.com/speech-api/v1/recognize?xjerr=1&client=chromium&lang=nl-NL"
#endif


SpeechToText::SpeechToText(Server& aserver,pool<utterancebuffer>& autterancepool) : Task(),server(aserver),utterancepool(autterancepool),flacbuffer(0),resultbuffer(0),fbs(0),rbs(0),sfd(0)
{
#ifdef USE_GOOGLE_STT
	flacbuffer = new unsigned char[FLACBUFFER_SIZE];
	resultbuffer = new unsigned char[RESULTBUFFER_SIZE];
	curl_global_init(CURL_GLOBAL_DEFAULT);
	printf("Initialized Google speech-to-text\n");
#else
	printf("No speech-to-text available\n");
#endif
}


SpeechToText::~SpeechToText()
{
#ifdef USE_GOOGLE_STT
	curl_global_cleanup();
	delete[] flacbuffer;
	delete[] resultbuffer;
#endif
}


void SpeechToText::Message(int msg,void* data)
{
	switch(msg)
	{
		case ITM_AUDIO:
			{
				//printf("SpeechToText: processing audiobuffer\n");
				audiobuffer* audio = (audiobuffer*)data;
				utterancebuffer* result = Process(audio);
				if(result)
				{
					//printf("SpeechToText: posting utterancebuffer to server\n");
					server.Post(ITM_UTTERANCE,result);
				}
				audio->release();
			}
			break;
	}
}


#ifdef USE_GOOGLE_STT
FLAC__StreamEncoderWriteStatus FLACWrite(const FLAC__StreamEncoder* encoder,const FLAC__byte buffer[],size_t bytes,unsigned samples,unsigned current_frame,void* client_data)
{
	SpeechToText* stt = (SpeechToText*)client_data;
	stt->AppendFLAC((unsigned char*)buffer,bytes);
    return FLAC__STREAM_ENCODER_WRITE_STATUS_OK;
}


size_t WriteMemoryCallback(void* contents,size_t size,size_t nmemb,void* userp)
{
	SpeechToText* stt = (SpeechToText*)userp;
	size_t realsize = size * nmemb;
	stt->AppendResult((unsigned char*)contents,realsize);
	return realsize;
}
#endif


utterancebuffer* SpeechToText::Process(audiobuffer* audio)
{
#ifdef USE_GOOGLE_STT
	fbs = 0;
	FLAC__StreamEncoder* encoder = 0;
	if(!(encoder = FLAC__stream_encoder_new()))
	{
		printf("SpeechToText::Process: cannot allocate FLAC encoder\n");
		return 0;
	}
	FLAC__stream_encoder_set_verify(encoder,true);
	FLAC__stream_encoder_set_compression_level(encoder,5);
	FLAC__stream_encoder_set_channels(encoder,1);
	FLAC__stream_encoder_set_bits_per_sample(encoder,16);
	FLAC__stream_encoder_set_sample_rate(encoder,22050);
	FLAC__StreamEncoderInitStatus init_status = FLAC__stream_encoder_init_stream(encoder,FLACWrite,0,0,0,this);
	if(init_status != FLAC__STREAM_ENCODER_INIT_STATUS_OK)
	{
		printf("SpeechToText::Process: cannot initialize FLAC encoder\n");
		return 0;
	}
	int rem = audio->frames;
	int total = 0;
	FLAC__int32 temp[CHUNK_SIZE];
	while(rem)
	{
		int chunk = rem;
		if(chunk > CHUNK_SIZE)
			chunk = CHUNK_SIZE;
		memset(temp,0,CHUNK_SIZE * sizeof(FLAC__int32));
        for(int i = 0; i < chunk; i++)
            temp[i] = audio->frame[total + i];
		FLAC__bool status = FLAC__stream_encoder_process_interleaved(encoder,temp,chunk);
		if(!status)
		{
			printf("SpeechToText::Process: FLAC encoding error\n");
			return 0;
		}
		rem -= chunk;
		total += chunk;
	}
	FLAC__stream_encoder_finish(encoder);
	FLAC__stream_encoder_delete(encoder);
	CURL* curl = curl_easy_init();
	if(!curl)
	{
		printf("SpeechToText::Process: cannot initialize CURL context\n");
		return 0;
	}
	rbs = 0;
	curl_easy_setopt(curl,CURLOPT_URL,GOOGLE_URL_NL);
	curl_easy_setopt(curl,CURLOPT_POST,1L);
	curl_easy_setopt(curl,CURLOPT_POSTFIELDSIZE,fbs);
	curl_easy_setopt(curl,CURLOPT_COPYPOSTFIELDS,flacbuffer);
	struct curl_slist* headers = NULL;
	headers = curl_slist_append(headers,"Content-type: audio/x-flac; rate=22050");
    curl_easy_setopt(curl,CURLOPT_HTTPHEADER,headers);
	curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,WriteMemoryCallback);
	curl_easy_setopt(curl,CURLOPT_WRITEDATA,this);
	curl_easy_setopt(curl,CURLOPT_TIMEOUT,5L);
	CURLcode res = curl_easy_perform(curl);
	if(res != CURLE_OK)
	{
		printf("SpeechToText::Process: CURL request failed (%s)\n",curl_easy_strerror(res));
		curl_easy_cleanup(curl);
	}
	else
	{
		resultbuffer[rbs] = 0;
		json_object* json = json_tokener_parse((char*)resultbuffer);
		json_object* status = json_object_object_get(json,"status");
		json_object* hypotheses = json_object_object_get(json,"hypotheses");
		if(!status || !hypotheses)
		{
			printf("SpeechToText::Process: JSON result corrupt\n");
			curl_easy_cleanup(curl);
			return 0;
		}
		if(json_object_get_int(status) != 0)
		{
			curl_easy_cleanup(curl);
			return 0;
		}
		utterancebuffer* result = utterancepool.get();
		result->options = json_object_array_length(hypotheses);
		for(int i = 0; i < result->options; i++)
		{
			json_object* hypothesis = json_object_array_get_idx(hypotheses,i);
			json_object* confidence = json_object_object_get(hypothesis,"confidence");
			json_object* utterance = json_object_object_get(hypothesis,"utterance");
			if(!confidence || !utterance)
			{
				printf("SpeechToText::Process: JSON result corrupt\n");
				result->release();
				curl_easy_cleanup(curl);
				return 0;
			}
			result->option[i].confidence = json_object_get_double(confidence);
			strncpy(result->option[i].data,json_object_get_string(utterance),MAX_TEXT_CHARACTERS);
		}
		curl_easy_cleanup(curl);
		return result;
	}
#endif
	return 0;
}


#ifdef USE_GOOGLE_STT
void SpeechToText::AppendFLAC(unsigned char* buffer,int length)
{
	if(fbs + length >= FLACBUFFER_SIZE)
		length = FLACBUFFER_SIZE - fbs;
	if(length > 0)
	{
		memcpy(&(flacbuffer[fbs]),buffer,length);
		fbs += length;
	}
}


void SpeechToText::AppendResult(unsigned char* buffer,int length)
{
	if(rbs + length >= RESULTBUFFER_SIZE)
		length = RESULTBUFFER_SIZE - rbs;
	if(length)
	{
		memcpy(&(resultbuffer[rbs]),buffer,length);
		rbs += length;
	}
}
#endif
