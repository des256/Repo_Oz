// OZ - Robot Puppeteering Server
// (C) Copyrights by Desmond Germans
// 2014 Germans Media Technology & Services

#ifndef _POOL_H_
#define _POOL_H_


// max. number of frames in audiobuffer
#define MAX_AUDIO_FRAMES (22050 * 10)

// max. number of faces recognized at once
#define MAX_FACES 16

// max. number of characters in text buffer
#define MAX_TEXT_CHARACTERS 1024

// max. number of options in utterance buffer
#define MAX_UTTERANCE_OPTIONS 16

// max. number of marks in speech buffer
#define MAX_SPEECH_MARKS 1024

// max. number of gestures in speech buffer
#define MAX_SPEECH_GESTURES 1024

// max. number of slots per animation track
#define MAX_TRACK_SLOTS 64


template<typename T> class pool
{
	pthread_mutex_t mutex;
	T* data;
	bool* used;
	int slots;

public:
	pool<T>(int aslots) : mutex(),data(0),used(0),slots(aslots)
	{
		pthread_mutex_init(&mutex,0);
		data = new T[slots];
		for(int i = 0; i < slots; i++)
			data[i].owner = this;
		used = new bool[slots];
		memset(used,0,slots * sizeof(bool));
	};

	~pool<T>()
	{
		delete[] data;
		delete[] used;
		pthread_mutex_destroy(&mutex);
	};

	T* get()
	{
		T* result = 0;
		pthread_mutex_lock(&mutex);
		int found;
		for(found = 0; found < slots; found++)
			if(!used[found])
				break;
		if(found >= slots)
			printf("Buffer overrun!!\n");
		else
		{
			used[found] = true;
			result = &(data[found]);
		}
		pthread_mutex_unlock(&mutex);
		return result;
	};

	void release(T* buffer)
	{
		pthread_mutex_lock(&mutex);
		int found;
		for(found = 0; found < slots; found++)
			if(&(data[found]) == buffer)
				break;
		if(found >= slots)
			printf("Wrong pool!!\n");
		else
			used[found] = false;
		pthread_mutex_unlock(&mutex);
	};
};


struct videobuffer
{
	pool<videobuffer>* owner;
	unsigned char data[VIDEO_FRAME];

	videobuffer() : owner(0) { };
	void reset() { };
	void release() { if(owner) owner->release(this); };
};


struct audiobuffer
{
	pool<audiobuffer>* owner;
	int frames;
	short frame[MAX_AUDIO_FRAMES];

	audiobuffer() : owner(0),frames(0) { };
	~audiobuffer() { };
	void reset() { frames = 0; };
	void release() { if(owner) owner->release(this); };
};


struct facebuffer
{
	pool<facebuffer>* owner;
	int faces;
	struct
	{
		intr rect;
		char identity[MAX_TEXT_CHARACTERS];
	} face[MAX_FACES];

	facebuffer() : owner(0),faces(0) { };
	void reset() { faces = 0; };
	void release() { if(owner) owner->release(this); };
};


struct textbuffer
{
	pool<textbuffer>* owner;
	char data[MAX_TEXT_CHARACTERS];

	textbuffer() : owner(0) { data[0] = 0; };
	void reset() { data[0] = 0; };
	void release() { if(owner) owner->release(this); };
};


struct utterancebuffer
{
	pool<utterancebuffer>* owner;
	int options;
	struct
	{
		float confidence;
		char data[MAX_TEXT_CHARACTERS];
	} option[MAX_UTTERANCE_OPTIONS];

	utterancebuffer() : owner(0),options(0) { };
	void reset() { options = 0; };
	void release() { if(owner) owner->release(this); };
};


struct speechbuffer
{
	pool<speechbuffer>* owner;
	int frames;
	short frame[MAX_AUDIO_FRAMES];
	int gestures;
	struct
	{
		unsigned long long ts;
		int id;
	} gesture[MAX_SPEECH_GESTURES];

	speechbuffer() : owner(0),frames(0),gestures(0) { };
	void reset() { frames = 0; gestures = 0; };
	void release() { if(owner) owner->release(this); };
};


struct lipsyncbuffer
{
	pool<lipsyncbuffer>* owner;
	int slots;
	struct
	{
		unsigned long long ts;
		float v;
	} slot[MAX_TRACK_SLOTS];

	lipsyncbuffer() : owner(0),slots(0) { };
	void reset() { slots = 0; };
	void release() { if(owner) owner->release(this); };
};


#endif  // _POOL_H_
