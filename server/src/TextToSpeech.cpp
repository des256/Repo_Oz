// OZ - Robot Puppeteering Server
// (C) Copyrights by Desmond Germans
// 2014 Germans Media Technology & Services

#include "TextToSpeech.h"
#include "Server.h"
#include "../../protocol.h"


#ifdef USE_ACAPELA_TTS

// text processing chunk size
#define CHUNK_SIZE 16384

// time shift for viseme animation
#define SPEAK_DELAY_MSEC 100

// time shift for gesture animation
#define GESTURE_DELAY_MSEC 200

// maximum length of a sample
#define MAX_SAMPLE_LENGTH (60 * 22050)

#define DBLSMAN_W_UNHANDLED	1
#define DBLSMAN_HANDLED		0
#define DBLSMAN_E_ERROR		-1


unsigned char* NewFile(const char* name)
{
	FILE* file = fopen(name,"rb");
	fseek(file,0,SEEK_END);
	unsigned long long size = ftell(file);
	fseek(file,0,SEEK_SET);
	unsigned char* buffer = new unsigned char[size];
	fread(buffer,size,1,file);
	fclose(file);
	return buffer;
}


int fillInitId(BB_DbLs* dbLsBlk, const char *Path,const char* descriptor,BB_DbMemType type,BB_U8 swapEndian)
{
	//long size=0xffffffff;
	dbLsBlk->pDbId=(BB_DbId*) malloc(sizeof(BB_DbId));
	if (!dbLsBlk->pDbId)
	{
		printf("Not Enough Memory!\n");
		return -1;
	}
	memset(dbLsBlk->pDbId,0,sizeof(BB_DbId));
#ifdef FILE_ACCESS_MEMORY
	dbLsBlk->pDbId->type=type;
#if defined (_UNICODE) && !defined(__SYMBIAN32__)
	if ((type&(BB_DBTYPE_FILE|BB_DBMEM_MEMORYMAP))
		&& ((dbLsBlk->pDbId->link=_strdup(Path))==0))
#else
	if ((type&(BB_DBTYPE_FILE|BB_DBMEM_MEMORYMAP))
		&& ((dbLsBlk->pDbId->link= (BB_TCHAR*) strdup((char*)Path))==0))
#endif
	{
		printf("%s\n",(char*)dbLsBlk->pDbId->link);
		printf("%s\n",(char*)Path);
		free(dbLsBlk->pDbId);
		return -1;
	}
	if ((type&(X_RAM_MASK|BB_DBMEM_MEMORYMAP))==X_RAM)
	{
		if ((dbLsBlk->pDbId->link=(BB_TCHAR*) NewFile(Path))==0)
		{
			printf("%s\n",(char*)Path);
			free(dbLsBlk->pDbId);
			return -1;
		}
	}
#else
	dbLsBlk->pDbId->type=X_ONLY_RAM;
    if ((dbLsBlk->pDbId->link=(BB_TCHAR*) NewFile(Path))==0)
	{
		printf("%s\n",Path);
		free(dbLsBlk->pDbId);
		return -1;
	}
#endif
	if (swapEndian!=0) dbLsBlk->pDbId->flags=BB_DBFLAGS_INVERSE;
	strncpy((char*)dbLsBlk->descriptor, (char*)descriptor, 4);
	return 0;
}

int fillInitStruct(const char* line, BB_DbLs* dbLsBlk, const char* accessPath, const char* descriptor, BB_DbMemType type, BB_U8 swapEndian)
{	/* this function fills the BB_DbLs structure */
	char fullPath[512];
	fullPath[0]=0;
	if (!line) return -1;
    if(accessPath)
    {
		int l=(int) strlen((const char*)accessPath);
		l=(l>0?l-1:0);
        strcpy((char*)fullPath, (char*)accessPath);
        if (fullPath[l]=='\\' || fullPath[l]=='/')
        {
			fullPath[l]='/';l++;
			fullPath[l]=0;
		}
	}
	strcat((char*)fullPath, (char*)line);
	unsigned int i;
	for (i=0;i<strlen(fullPath);i++) {if (fullPath[i]=='\\') {fullPath[i]='/';}}
	return fillInitId(dbLsBlk,fullPath,descriptor,type,swapEndian);
}

void destroyInitStruct(BB_DbLs* dbLsBlk)
{
	if (dbLsBlk)
	{
		if (dbLsBlk->pDbId)
		{
			if (dbLsBlk->pDbId->link)
			{
				free(dbLsBlk->pDbId->link);
			}
			free(dbLsBlk->pDbId);
			dbLsBlk->pDbId=NULL;
		}
	}
}

void destroyLanguageDba(BB_DbLs* ldba)
{
	int i;
	if (ldba)
	{
		for (i=0; ldba[i].descriptor[0]; i++)
		{
			destroyInitStruct(&ldba[i]);
		}
		free(ldba);
	}
}



BB_DbLs* initLanguageDbaExEx(FILE* file,const char* name,const char* path,int nitem,int* error,unsigned short type,int forceType, int (*callback) (char* prefix,char* valeur, char* options ,char* accesspath, void* object),void* obj)
{
	FILE* f=file;
	BB_DbLs* nlpDba;
	char strType_[32];
	char line[256];
	char decl[256];
	char accesspath[256];
	BB_S32 i, ascan;
	BB_U16 useType;
	BB_U8 swapEndian=0;
	char* strType=strType_;
	*accesspath=0;
	if (error) *error=0;
	if (!f && !name) { if (error) *error=-100;return 0;}
	if (path) strncpy((char*)accesspath,(char*)path,sizeof(accesspath));
	if (*accesspath)
	{
#if defined(WIN32)
		if ('\\' != accesspath[strlen(accesspath) - 1])
			strncat(accesspath, "\\", (sizeof(accesspath) / sizeof(accesspath[0])) - 1);
#else
		if ('/' != accesspath[strlen(accesspath) - 1])
			strncat(accesspath, "/", (sizeof(accesspath) / sizeof(accesspath[0])) - 1);
#endif
	}
	if ((!f) && (name!=NULL) && (strlen(name)!=0)) f=fopen(name,"rb");		/* open the ini file */
	if (!f)
	{
		if (error)	*error=-1;
		return 0;
	}
	/* Reading number of entries */
	if (nitem<=0)
	{
		nitem=0;
		while (fgets(line,sizeof(line),f)!=NULL)
		{
			if (sscanf((const char*)line,"%s \"%[^\"]\"",(char*)decl,(char*)line)==2 && *line ) nitem++;
		}
	}
	/* allocating structure */
	nlpDba=(BB_DbLs*) malloc(sizeof(BB_DbLs)*(nitem+1));
	if (!nlpDba) {fclose(f); if (error)	*error=-2; return 0;}
	memset(nlpDba,0, sizeof(BB_DbLs)*(nitem+1));
	/* loop on init file */
	if (error) *error=-10;
	i=0;
	fseek(f,0,SEEK_SET);
	while (i<nitem)
	{
		strType=strType_;
		*strType=0;
		if (error) (*error)--;
		if (fgets((char*)line,sizeof(line),f)==NULL)
		{
			break;
		}
		if (sscanf((char*)line,"PATH %s \"%[^\"]\"", (char*)decl, (char*)line) == 2)
		{
			if (*line==' ') strcpy((char*)accesspath,(char*)&line[1]);
			else strcpy((char*)accesspath,(char*)line);
		}

		if (sscanf((char*)line,"%s = \"%[^\"]\"", (char*)decl, (char*)line) == 2)
		{
			if (callback) callback((char*)decl,(char*)line,NULL,(char*)accesspath, obj);
		}

		if ((ascan=sscanf((const char*)line,"%s \"%[^\"]\" [ %[^] ] ] ", (char*)decl, (char*)line, (char*)strType))>=2)
		{
			if (!callback || callback((char*)decl,(char*)line,(char*)strType,(char*)accesspath, obj)!=0)
			{
				useType=type;
				swapEndian=0;
				if (ascan==3 && !strncmp((char*)strType,"ENDIAN_",7)) { swapEndian=1; strType+=7; }
				if (ascan==3 && !strncmp((char*)strType,"RAM",3)) useType=X_RAM;
				if (ascan==3 && !strncmp((char*)strType,"RAM_PROTECTED",13)) useType=X_RAM|BB_DBSIZEPROTECT_ON;
				if (ascan==3 && !strncmp((char*)strType,"FILE",4)) useType=X_FILE;
				if (ascan==3 && !strncmp((char*)strType,"FILEMAP",7)) useType=X_FILEMAP;
				if (ascan==3 && !strncmp((char*)strType,"FILEMAP_PROTECTED",17)) useType=X_FILEMAP|BB_DBSIZEPROTECT_ON;
				if (forceType) useType=type; /* Force configuration (non optimal) */
				if (!strcmp((char*)decl,"BNF"))	/* BNF DEFINITIONS */
				{
					if (fillInitStruct(line,&nlpDba[i],accesspath,decl,X_RAM,swapEndian)<0)
					{
						destroyLanguageDba(nlpDba);fclose(f); return 0;
					}
				}
				else if (!strcmp((char*)decl,"IVX"))	/* IVX  */
				{
					if (fillInitStruct(line,&nlpDba[i],accesspath,decl,X_RAM,swapEndian)<0)
					{
						destroyLanguageDba(nlpDba);fclose(f); return 0;
					}
				}
				else if (!strcmp((char*)decl,"PHM"))	/* PHM  */
				{
					if (fillInitStruct(line,&nlpDba[i],accesspath,decl,X_RAM,swapEndian)<0)
					{
						destroyLanguageDba(nlpDba);fclose(f); return 0;
					}
				}
				else if (!strcmp((char*)decl,"END"))	/* End marker  */
				{
					if (fillInitStruct(line,&nlpDba[i],accesspath,decl,useType,swapEndian)<0)
					{
						destroyLanguageDba(nlpDba);fclose(f); return 0;
					}
				}
				else
				{
					if (fillInitStruct(line,&nlpDba[i],accesspath,decl,useType,swapEndian)<0)
					{
						destroyLanguageDba(nlpDba);fclose(f); return 0;
					}
				}
				i++;
			}
		}
	}
	if (error) *error=0;
	fclose(f);
	return nlpDba;
}

BB_DbLs* initLanguageDbaEx(FILE* file,const char* name,const char* path,int nitem,int* error,unsigned short type,int forceType)
{
	return initLanguageDbaExEx( file,name, path, nitem, error,type, forceType,NULL,0);
}

BB_DbLs* initLanguageDba(FILE* file,const char* name,const char* path,int nitem,int* error,unsigned short type)
{
	return initLanguageDbaEx(file,name,path,nitem,error,type,0);
}

static BB_DbLs* BABILE_loadIniFile( char const * const aFilename,BB_DbLs** ppNLPLs,BB_DbLs** ppSynthLS,short* nlpModule,BB_S32* synthAvailable,short* synthActif, const char** pText, unsigned short defaultLoad, unsigned short overrideLoad )
{
	int error=0,i;
	BB_DbLs* iData=NULL;
	char  * chainePath;
	if (defaultLoad==0) defaultLoad=X_RAM;

	if (!aFilename || !ppNLPLs || !ppSynthLS || !nlpModule || !synthAvailable || !synthActif || !pText)
	{
		return NULL;
	}
	/* GET PATH */
	chainePath=strdup(aFilename);
	if (chainePath)
	{
		for (i=(int) strlen(chainePath); i>=0; i--)
		{
			if ((chainePath[i]=='\\') ||
				(chainePath[i]=='/'))
				break;
		}
		chainePath[i+1]=0;
	}
	iData=initLanguageDbaEx(NULL,aFilename,chainePath,0,&error,defaultLoad /*default type */,overrideLoad);
	if (chainePath) free(chainePath);
	if (iData)
	{
		int vi;
		BB_DbLs* voiceData=0;
		*synthActif=BABILE_SYNTHACTIVE_NONE;
		*synthAvailable=0;
		*nlpModule=BABILE_NLPMODULE_NLPE;
		/* CHECK for MBROLA */
		for (vi=0; iData[vi].descriptor[0] && strcmp((char*)iData[vi].descriptor,"MBR") ;vi++);
		if (iData[vi].descriptor[0])
		{
			voiceData=&iData[vi];
			*synthActif=BABILE_SYNTHACTIVE_MBROLA;
			*synthAvailable|=BABILE_SYNTHMODULE_MBROLA;
		}
		/* CHECK for SELECTOR */
		for (vi=0; iData[vi].descriptor[0] && (
				strcmp((char*)iData[vi].descriptor,"SLV") &&
				strcmp((char*)iData[vi].descriptor,"SLL")
				) ;vi++);
		if (iData[vi].descriptor[0])
		{
			voiceData=&iData[vi];
			*synthActif=BABILE_SYNTHACTIVE_BBSELECTOR;
			*synthAvailable|= BABILE_SYNTHMODULE_BBSELECTOR;
		}
		/* CHECK for COLIBRI */
		for (vi=0; iData[vi].descriptor[0] && (
				strcmp((char*)iData[vi].descriptor,"ALF") &&
				strcmp((char*)iData[vi].descriptor,"ERF")
				) ;vi++);
		if (iData[vi].descriptor[0])
		{
			voiceData=&iData[vi];
			*synthActif=BABILE_SYNTHACTIVE_COLIBRI;
			*synthAvailable|= BABILE_SYNTHMODULE_COLIBRI;
		}
		/* CHECK for TTF */
		for (vi=0; iData[vi].descriptor[0] && (
				strcmp((char*)iData[vi].descriptor,"IVX")
				) ;vi++);
		if (iData[vi].descriptor[0])
		{
			*nlpModule=BABILE_NLPMODULE_TTF;
		}
		/* CHECK for NLPE */
		for (vi=0; iData[vi].descriptor[0] && (
				strcmp((char*)iData[vi].descriptor,"PHO")
				) ;vi++);
		if (iData[vi].descriptor[0])
		{
			*nlpModule=BABILE_NLPMODULE_NLPE;
		}
		/* CHECK for TEXT */
		for (vi=0; iData[vi].descriptor[0] && (
				strcmp((const char*)iData[vi].descriptor,"TXT")
				) ;vi++);
		if (iData[vi].descriptor[0] && iData[vi].pDbId->type==X_RAM )
		{
			*pText = (const char*)iData[vi].pDbId->link;
		}
		*ppSynthLS = voiceData;
		*ppNLPLs=iData;
	}
	return iData;
}


TextToSpeech* global_tts = 0;


BB_S32 MarkCallback(BABILE_EventCallBackStruct* info)
{
	//printf("[%05d] ",info->off);
	switch(info->event_type)
	{
		case BABILE_SYNCPHOCALL:  // new phoneme
			if(global_tts->visemes < MAX_VISEMES)
			{
				global_tts->vts[global_tts->visemes] = info->off;
				global_tts->vid[global_tts->visemes] = info->viseme;
				global_tts->visemes++;
			}
			break;

		case BABILE_SYNCWORCALL:  // new word
			if(global_tts->words < MAX_WORDS)
			{
				global_tts->wcp[global_tts->words] = info->pos;
				global_tts->wts[global_tts->words] = info->off;
				global_tts->words++;
			}
			break;

		case BABILE_SYNCBRECALL:  // new breath group
			printf("new breath group\n");
			break;

		case BABILE_SYNCSENCALL:  // new breath sentence
			printf("new breath sentence\n");
			break;
	}
	return 0;
}
#endif


#ifdef USE_IVONA_TTS
// text processing chunk size
#define CHUNK_SIZE 16384

// time shift for viseme animation
#define SPEAK_DELAY_MSEC 400

// maximum length of a sample
#define MAX_SAMPLE_LENGTH (60 * 22050)
#endif


TextToSpeech::TextToSpeech(Server& aserver,Speaker& aspeaker,Servos& aservos,Microphone& amicrophone,pool<speechbuffer>& aspeechpool,pool<lipsyncbuffer>& alipsyncpool) : Task(),server(aserver),speaker(aspeaker),servos(aservos),microphone(amicrophone),speechpool(aspeechpool),lipsyncpool(alipsyncpool)
{
#ifdef USE_ACAPELA_TTS
	global_tts = this;
	BB_S32 rate = 22050;
	BB_DbLs* pNLPLs = 0;
	BB_DbLs* pSynthLs = 0;
	short nlpActif = 0;
	short synthActif = 0;
	BB_S32 synthAvailable = 0;
	char* defaultText = 0;

	// load voice-specific data
	voice = BABILE_loadIniFile(ACAPELA_VOICE,&pNLPLs,&pSynthLs,&nlpActif,&synthAvailable,&synthActif,(const char**)&defaultText,X_RAM,0);
	if(!voice)
	{
		printf("Unable to load voice-specific data, so no text-to-speech available\n");
		return;
	}

	// initialize parameters
	BABILE_MemParam params;
	memset(&params,0,sizeof(BABILE_MemParam));
	params.sSize = sizeof(BABILE_MemParam);

	// setup license info
	params.license = (BB_TCHAR*)ACAPELA_LICENSE;
	params.uid.userId = ACAPELA_USERID;
	params.uid.passwd = ACAPELA_PASSWORD;

	// setup rest from voice specification
	params.nlpeLS = pNLPLs;
	params.nlpModule = nlpActif;
	params.synthLS = pSynthLs;
	params.synthModule = synthAvailable;

	// and set the callback
	params.markCallback = MarkCallback;
	params.u32MarkCallbackInstance = 0;

	// initialize Acapela
	bab = BABILE_initEx(&params);
	if(!bab)
	{
		printf("Cannot initialize Acapela, so no text-to-speech available\n");
		return;
	}
	BABILE_getSetting(bab,BABIL_PARM_VOICEFREQ,&rate);
	BABILE_setSetting(bab,BABIL_PARM_SYNC,BABIL_PARM_SEL_VAL_SYNCONPHONEMES | BABIL_PARM_SEL_VAL_MBR_SYNCONWORDS);

	printf("Initialized Acapela text-to-speech\n");
#else
#ifdef USE_IVONA_TTS
    // create instance
    if(!(instance = tts_create()))
    {
        printf("Cannot create Ivona instance, so no text-to-speech available\n");
        return;
    }

    // load certificate
    if(!tts_load_certificate_file(instance,IVONA_CERTIFICATE_FILE))
    {
        printf("Certificate problem (certificate \"%s\"), so no text-to-speech available\n",IVONA_CERTIFICATE_FILE);
        return;
    }

    // load voice
    if(!(voice = tts_load_voice(instance,IVONA_VOICE_LIB,IVONA_VOICE_DB)))
    {
    	printf("Cannot load voice (voice library \"%s\", voice database \"%s\"), so no text-to-speech available\n",IVONA_VOICE_LIB,IVONA_VOICE_DB);
        return;
    }
	printf("Initialized Ivona text-to-speech\n");
#else
	printf("No text-to-speech available\n");
#endif
#endif
}


TextToSpeech::~TextToSpeech()
{
#ifdef USE_ACAPELA_TTS
	if(bab)
		BABILE_freeEx(bab);
	if(voice)
	{
		for(int i = 0; voice[i].descriptor[0]; i++)
			if(voice[i].pDbId)
			{
				if(voice[i].pDbId->link)
					free(voice[i].pDbId->link);
				free(voice[i].pDbId);
				voice[i].pDbId = NULL;
			}
		free(voice);
	}
#endif
#ifdef USE_IVONA_TTS
    if(voice)
        tts_unload_voice(voice);
    if(instance)
        tts_destroy(instance);
#endif
}


void TextToSpeech::Message(int msg,void* data)
{
	switch(msg)
	{
		case ITM_TEXT:
			{
				textbuffer* text = (textbuffer*)data;
				lipsyncbuffer* lipsync = 0;
				speechbuffer* result = Process(text,lipsync);
				if(result)
				{
					// mute microphone
					microphone.Post(ITM_MUTE,0);

					// calculate length of sample
					int msec = (result->frames * 1000) / 22050;

					// adjust timestamps to current time plus speak delay
					struct timeval tv;
					gettimeofday(&tv,0);
					unsigned long long now_ts = (tv.tv_sec * 1000) + ((tv.tv_usec / 1000) % 1000);
					for(int i = 0; i < lipsync->slots; i++)
						lipsync->slot[i].ts += SPEAK_DELAY_MSEC + now_ts;

					// post lipsync to servos
					servos.Post(ITM_LIPSYNC,lipsync);

					// post sample to speaker
					speaker.Post(ITM_SPEECH,result);

					// process the gestures
					for(int i = 0; i < result->gestures; i++)
					{
						// wait for gesture moment
						struct timeval tv;
						gettimeofday(&tv,0);
						unsigned long long ts = (tv.tv_sec * 1000) + ((tv.tv_usec / 1000) % 1000);
						printf("    current time = %llu\n",ts);
						unsigned long long need_ts = result->gesture[i].ts + GESTURE_DELAY_MSEC + now_ts;
						printf("    gesture should start at %llu\n",need_ts);
						unsigned long long diff = need_ts - ts;
						if(diff < 10000)  // anything too high is ignored (overflow means no wait needed)
						{
							printf("    so we wait %llu msec\n",diff);
							usleep(diff * 1000);
						}

						// post gesture
						servos.Post(ITM_GESTURE,(void*)(result->gesture[i].id));
					}

					// wait for text to finish
					gettimeofday(&tv,0);
					unsigned long long ts = (tv.tv_sec * 1000) + ((tv.tv_usec / 1000) % 1000);
					printf("    current time = %llu\n",ts);
					unsigned long long diff = now_ts + msec - ts;
					printf("    speech should end at %llu\n",now_ts + msec);
					if(diff < 10000)
					{
						printf("    so we wait %llu msec\n",diff);
						usleep(diff * 1000);
					}

					// unmute microphone
					microphone.Post(ITM_UNMUTE,0);

					// and signal server that speech is done
					server.Post(ITM_SPEECH_DONE,0);
				}
				text->release();
			}
			break;
	}
}


#ifdef USE_ACAPELA_TTS
float viseme_value[22] =  // Microsoft SAPI/Disney
{
	0.0f,  // silence
	1.0f,  // ae ax ah
	0.9f,  // aa
	0.7f,  // ao
	0.8f,  // ey eh uh
	0.4f,  // er
	0.3f,  // y iy ih ix
	0.1f,  // w uw
	0.5f,  // ow
	0.7f,  // aw
	0.4f,  // oy
	0.6f,  // ay
	0.5f,  // h
	0.1f,  // r
	0.1f,  // l
	0.1f,  // s z
	0.2f,  // sh ch jh zh
	0.2f,  // th dh
	0.1f,  // f v
	0.2f,  // d t n
	0.3f,  // k g ng
	0.0f,  // p b m
};
#endif


void AppendGesture(speechbuffer* speech,int charpos,int id)
{
	speech->gesture[speech->gestures].ts = charpos;
	speech->gesture[speech->gestures].id = id;
	(speech->gestures)++;
}


enum
{
	TOKEN_EOL = 0,
	TOKEN_WORD,
	TOKEN_GESTURE,
	TOKEN_COMMA
};


int GetToken(char*& cptr,char*& sptr)
{
	while(true)
	{
		while((*cptr == ' ') || (*cptr == '\t'))
			cptr++;
		if(*cptr == 0)
			return TOKEN_EOL;
		else if(*cptr == '[')
		{
			cptr++;
			while((*cptr != ']') && (*cptr != 0))
				*sptr++ = *cptr++;
			*sptr++ = 0;
			if(*cptr == 0)
				return TOKEN_EOL;
			cptr++;
			return TOKEN_GESTURE;
		}
		else if(*cptr == ',')
		{
			cptr++;
			return TOKEN_COMMA;
		}
		else if(((*cptr >= 'a') && (*cptr <= 'z')) || ((*cptr >= 'A') && (*cptr <= 'Z')) || ((*cptr >= '0') && (*cptr <= '9')))
		{
			while(((*cptr >= 'a') && (*cptr <= 'z')) || ((*cptr >= 'A') && (*cptr <= 'Z')) || ((*cptr >= '0') && (*cptr <= '9')))
				*sptr++ = *cptr++;
			*sptr++ = 0;
			return TOKEN_WORD;
		}
		else
			printf("unknown character \'%c\'\n",*cptr++);
	}
}


speechbuffer* TextToSpeech::Process(textbuffer* text,lipsyncbuffer*& lipsync)
{
	speechbuffer* result = speechpool.get();
	result->gestures = 0;

	// TODO: preprocess text, remove articulations and gestures, put in list
	char* cptr = text->data;
	char str[4096];
	char target[4096];
	memset(target,0,4096);

	int token;
	bool need_space = false;
	do
	{
		char* sptr = str;
		token = GetToken(cptr,sptr);
		if(token == TOKEN_EOL)
			break;
		switch(token)
		{
			case TOKEN_WORD:
				if(need_space)
					strcat(target," ");
				strcat(target,str);
				need_space = true;
				break;

			case TOKEN_GESTURE:
				if(!strcasecmp(str,"braisel"))
					AppendGesture(result,strlen(target),oz::GESTURE_BROW_RAISE_LONG);
				if(!strcasecmp(str,"braises"))
					AppendGesture(result,strlen(target),oz::GESTURE_BROW_RAISE_SHORT);
				if(!strcasecmp(str,"frownl"))
					AppendGesture(result,strlen(target),oz::GESTURE_FROWN_LONG);
				if(!strcasecmp(str,"frowns"))
					AppendGesture(result,strlen(target),oz::GESTURE_FROWN_SHORT);
				if(!strcasecmp(str,"yesl"))
					AppendGesture(result,strlen(target),oz::GESTURE_YES_LONG);
				if(!strcasecmp(str,"yess"))
					AppendGesture(result,strlen(target),oz::GESTURE_YES_SHORT);
				if(!strcasecmp(str,"yesl3"))
					AppendGesture(result,strlen(target),oz::GESTURE_YES_LONG_3X);
				if(!strcasecmp(str,"yess3"))
					AppendGesture(result,strlen(target),oz::GESTURE_YES_SHORT_3X);
				if(!strcasecmp(str,"nol"))
					AppendGesture(result,strlen(target),oz::GESTURE_NO_LONG);
				if(!strcasecmp(str,"nos"))
					AppendGesture(result,strlen(target),oz::GESTURE_NO_SHORT);
				if(!strcasecmp(str,"nol3"))
					AppendGesture(result,strlen(target),oz::GESTURE_NO_LONG_3X);
				if(!strcasecmp(str,"nos3"))
					AppendGesture(result,strlen(target),oz::GESTURE_NO_SHORT_3X);
				if(!strcasecmp(str,"wob"))
					AppendGesture(result,strlen(target),oz::GESTURE_INDIAN_WOBBLE);
				break;

			case TOKEN_COMMA:
				strcat(target,", ");
				need_space = false;
				break;
		}
	} while(true);

#ifdef USE_ACAPELA_TTS
	lipsync = lipsyncpool.get();
	lipsync->slots = 0;
	int total = 0;
	int pos = 0;
	BB_U32 samples = 0;
	do
	{
		if(total + CHUNK_SIZE > MAX_AUDIO_FRAMES)
			break;

		// process chunk
		visemes = 0;
		words = 0;
		pos += BABILE_readText(bab,(BB_TCHAR*)(target + pos),(BB_S16*)(result->frame + total),CHUNK_SIZE,&samples);

		// read back visemes
		for(int i = 0; i < visemes; i++)
			if(lipsync->slots <= MAX_TRACK_SLOTS)
			{
				unsigned long long ts = ((total + vts[i]) * 1000) / 22050;
				float v = 0.0f;
				if((vid[i] >= 0) && (vid[i] < 22))
					v = viseme_value[vid[i]];
				lipsync->slot[lipsync->slots].ts = ts;
				lipsync->slot[lipsync->slots].v = v;
				lipsync->slots++;
			}

		// position gesture positions accordingly
		for(int i = 0; i < words; i++)
			for(int k = 0; k < result->gestures; k++)
				if(result->gesture[k].ts == wcp[i])
					result->gesture[k].ts = ((total + wts[i]) * 1000) / 22050;
        	total += samples;
	} while(samples > 0);
	result->frames = total;
    	return result;
#endif
#ifdef USE_IVONA_TTS
    tts_streamer* streamer = tts_streamer_start(voice,text.c_str(),-1.0f);
    if(!streamer)
    {
    	printf("TextToSpeech::Process: cannot create streamer for text \"%s\"\n",text);
        return 0;
    }
	speechbuffer* result = speechpool.get();
    int total = 0;
    int synthesized = 0;
    do
    {
        synthesized = 0;
        if(tts_wave* wave = tts_streamer_synth(streamer,CHUNK_SIZE))
        {
            /*for(int i = 0; i < (int)wave->num_marks; i++)
                if(wave->marks[i].type == TTS_MARK_VISEME)
                {
                	result->mark[result->marks].ts = ts;
                    if(!strcmp(wave->marks[i].name,"sil"))
                    	result->mark[result->marks].id = MARK_SILENCE;
                    else
                        switch(wave->marks[i].name[0])
                        {
                            case '@': marks[ts] = MARK_A; break;
                            case 'a': marks[ts] = MARK_A; break;
                            case 'E': marks[ts] = MARK_E; break;
                            case 'e': marks[ts] = MARK_E; break;
                            case 'f': marks[ts] = MARK_PBFV; break;
                            case 'i': marks[ts] = MARK_I; break;
                            case 'k': marks[ts] = MARK_KH; break;
                            case 'O': marks[ts] = MARK_O; break;
                            case 'o': marks[ts] = MARK_O; break;
                            case 'p': marks[ts] = MARK_PBFV; break;
                            case 'r': marks[ts] = MARK_R; break;
                            case 'S': marks[ts] = MARK_SZ; break;
                            case 's': marks[ts] = MARK_SZ; break;
                            case 't': marks[ts] = MARK_TD; break;
                            case 'u': marks[ts] = MARK_U; break;

                            default:
                                printf("Ivona::ProcessText: unknown viseme mark \"%s\" at %d\n",wave->marks[i].name,(int)ts);
                                break;
                        }
                }*/

            synthesized = wave->num_samples;
            memcpy(&(result->data[total]),wave->samples,synthesized * 2);
            tts_wave_delete(wave);
            total += synthesized;
        }
    }
    while(synthesized > 0);
    tts_streamer_stop(streamer);
    return result;
#endif
}
