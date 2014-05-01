// OZ - Robot Puppeteering Server
// (C) Copyrights by Desmond Germans
// 2014 Germans Media Technology & Services

#ifndef _SERVOS_H_
#define _SERVOS_H_

#include "Task.h"
#include "pool.h"
#include "../../protocol.h"


// maximum number of gesture instances
#define MAX_INSTANCES 64


struct gesture_master
{
	int tracks;
	struct
	{
		int id;
		int slots;
		struct
		{
			unsigned long long ts;
			float v;
		} slot[MAX_TRACK_SLOTS];
	} track[oz::MAX_SERVOS];

	gesture_master() : tracks(0) { };
};


struct gesture_instance
{
	gesture_master* gt;
	unsigned long long ts;

	gesture_instance() : gt(0),ts(0) { };
};


class Server;
class Servos : public Task
{
	Server& server;                           // owning server reference
	int pfd;                                  // Pololu (head) fd
	int dfd;                                  // Dynamixel (body) fd
	int tfd;                                  // timer fd
	lipsyncbuffer* lipsync;                   // currently active lipsync animation
	float bc;                                 // blink counter (in ticks)
	int expression;                           // current expression (one of oz::EXPR_*)
	float expression_weight;                  // expression weight (0..1)
	float gesture_weight;                     // gesture weight (0..1)
	float auto_blink_speed;                   // auto blink speed (in Hz)
	float random_head_weight;                 // random head weight (0..1)
	float random_eyes_weight;                 // random eyes weight (0..1)
	float random_face_weight;                 // random face weight (0..1)
	float direct_weight;                      // direct servo control weight (0..1)
	int lookat;                               // current lookat (one of oz::LOOKAT_*)
	float lookat_weight;                      // lookat weight (0..1)

	gesture_master master[oz::MAX_GESTURES];  // gesture library

	int gestures;                             // number of currently active gestures
	gesture_instance gesture[MAX_INSTANCES];  // currently active gestures

	float random_head_yaw;
	float random_head_pitch;
	float random_head_roll;
	float random_eyes_yaw;
	float random_eyes_pitch;
	float random_face_brows;
	float random_face_smile;

public:
	Servos(Server& aserver,char* headname,char* bodyname);
	virtual ~Servos();
	virtual void Step();
	virtual void Message(int msg,void* data);

	void Reset();

	void SetHeadServo(int id,int pos);
	int GetHeadServo(int id);
	void SetHeadServos(int id,int n,int* pos);
	void SetBodyServo(int id,int pos);
	int GetBodyServo(int id);
	void SetBodyServos(int id,int n,int* pos);
	void SetHandServo(int id,int pos);
	int GetHandServo(int id);
	void SetHandServos(int id,int n,int* pos);

	bool AddGesture(float* pos,unsigned long long ts,gesture_instance* inst);
	bool AddLipsync(float* pos,unsigned long long ts);
};


#endif
