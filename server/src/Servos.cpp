// OZ - Robot Puppeteering Server
// (C) Copyrights by Desmond Germans
// 2014 Germans Media Technology & Services

#include "Servos.h"
#include "Server.h"


#define RANDOM_HEAD_CHANCE 10
#define RANDOM_EYES_CHANCE 10
#define RANDOM_FACE_CHANCE 20

#define RANDOM_HEAD_YAW_RANGE 0.2f
#define RANDOM_HEAD_PITCH_RANGE 0.2f
#define RANDOM_HEAD_ROLL_RANGE 0.2f

#define RANDOM_EYES_YAW_RANGE 0.3f
#define RANDOM_EYES_PITCH_RANGE 0.3f

#define RANDOM_FACE_BROWS_RANGE 0.3f
#define RANDOM_FACE_SMILE_RANGE 0.3f

#define EXPRESSION_BROWS_RANGE 1.0f
#define EXPRESSION_SMILE_RANGE 1.0f


struct servolimit
{
	int id;     // controller ID of the servo
	int lo,hi;  // low and high limit
	int home;   // home value
};


/*// servo limits for Ecila (taken straight from robot.xml)
servolimit limits[] =
{
//   id    lo    hi    home
	{0,    434,  594,  514},   // 0. waist
	{1,    970,  50,   810},   // 1. left shoulder pitch
	{2,    230,  512,  237},   // 2. left shoulder roll
	{3,    840,  220,  582},   // 3. left elbow yaw
	{4,    535,  830,  573},   // 4. left elbow pitch
	{5,    50,   970,  210},   // 5. right shoulder pitch
	{6,    795,  512,  787},   // 6. right shoulder roll
	{7,    200,  820,  458},   // 7. right elbow yaw
	{8,    530,  830,  568},   // 8. right elbow pitch
	{9,    525,  870,  590},   // 9. left hip roll
	{10,   624,  204,  512},   // 10. left hip yaw
	{11,   470,  825,  525},   // 11. left hip pitch
	{12,   820,  480,  512},   // 12. left knee pitch
	{13,   400,  745,  512},   // 13. left ankle pitch
	{14,   624,  404,  512},   // 14. left ankle roll
	{15,   645,  300,  595},   // 15. right hip roll
	{16,   400,  820,  512},   // 16. right hip yaw
	{17,   470,  825,  525},   // 17. right hip pitch
	{18,   820,  480,  512},   // 18. right knee pitch
	{19,   400,  745,  512},   // 19. right ankle pitch
	{20,   400,  620,  512},   // 20. right ankle roll

	{0,    1952, 1232, 1589},  // 21. neck yaw
	{1,    1120, 1872, 1492},  // 22. neck roll
	{2,    2208, 928,  1524},  // 23. neck pitch
	{3,    1136, 1952, 1559},  // 24. brows pitch
	{4,    1904, 1092, 1092},  // 25. eyelids
	{5,    1408, 1760, 1631},  // 26. eyes pitch
	{6,    1472, 1750, 1472},  // 27. jaw
	{8,    1408, 1568, 1488},  // 28. eye right
	{9,    1744, 1232, 1465},  // 29. smile left (lo and hi reversed from robot.xml)
	{10,   1440, 1600, 1520},  // 30. eye left
	{11,   1280, 1696, 1491},  // 31. smile right (lo and hi reversed from robot.xml)

	{7953, 355,  653,  504},   // 32. left wrist
	{7952, 599,  448,  523},   // 33. left grasp
	{7697, 654,  352,  503},   // 34. right wrist
	{7696, 428,  578,  503}    // 35. right grasp
};*/


// servo limits for Alice (taken straight from robot.xml)
servolimit limits[] =
{
//   id    lo    hi    home
	{0,    434,  594,  514},   // 0. waist
	{1,    970,  50,   810},   // 1. left shoulder pitch
	{2,    230,  512,  237},   // 2. left shoulder roll
	{3,    840,  220,  582},   // 3. left elbow yaw
	{4,    535,  830,  573},   // 4. left elbow pitch
	{5,    50,   970,  210},   // 5. right shoulder pitch
	{6,    795,  512,  787},   // 6. right shoulder roll
	{7,    200,  820,  458},   // 7. right elbow yaw
	{8,    530,  830,  568},   // 8. right elbow pitch
	{9,    525,  870,  590},   // 9. left hip roll
	{10,   624,  204,  512},   // 10. left hip yaw
	{11,   470,  825,  525},   // 11. left hip pitch
	{12,   820,  480,  512},   // 12. left knee pitch
	{13,   400,  745,  512},   // 13. left ankle pitch
	{14,   624,  404,  512},   // 14. left ankle roll
	{15,   645,  300,  595},   // 15. right hip roll
	{16,   400,  820,  512},   // 16. right hip yaw
	{17,   470,  825,  525},   // 17. right hip pitch
	{18,   820,  480,  512},   // 18. right knee pitch
	{19,   400,  745,  512},   // 19. right ankle pitch
	{20,   400,  620,  512},   // 20. right ankle roll

	{0,    1952, 1232, 1504},  // 21. neck yaw
	{1,    1120, 1872, 1500},  // 22. neck roll
	{2,    2208, 928,  1500},  // 23. neck pitch
	{3,    1136, 1952, 1500},  // 24. brows pitch
	{4,    1904, 1392, 1600},  // 25. eyelids
	{5,    1408, 1760, 1648},  // 26. eyes pitch
	{6,    1472, 1616, 1536},  // 27. jaw
	{8,    1408, 1568, 1488},  // 28. eye right
	{9,    1232, 1744, 1392},  // 29. smile left (lo and hi reversed from robot.xml)
	{10,   1440, 1600, 1520},  // 30. eye left
	{11,   1696, 1280, 1296},  // 31. smile right (lo and hi reversed from robot.xml)

	{7953, 355,  653,  504},   // 32. left wrist
	{7952, 599,  448,  523},   // 33. left grasp
	{7697, 654,  352,  503},   // 34. right wrist
	{7696, 428,  578,  503}    // 35. right grasp
};



#define SERVOS_MSEC 50


Servos::Servos(Server& aserver,char* headname,char* bodyname) : Task(),server(aserver),pfd(-1),dfd(-1),tfd(-1)
{
	// start timer
	tfd = timerfd_create(CLOCK_REALTIME,0);
	struct itimerspec its;
	its.it_interval.tv_sec = SERVOS_MSEC / 1000;
	its.it_interval.tv_nsec = (SERVOS_MSEC % 1000) * 1000000;
	its.it_value.tv_sec = SERVOS_MSEC / 1000;
	its.it_value.tv_nsec = (SERVOS_MSEC % 1000) * 1000000;
	timerfd_settime(tfd,0,&its,0);

	// open head servo connection
	pfd = open(headname,O_RDWR | O_NOCTTY | O_SYNC);
	if(pfd != -1)
	{
		struct termios tty;
		memset(&tty,0,sizeof(struct termios));
		tcgetattr(pfd,&tty);
		cfsetospeed(&tty,B115200);
		cfsetispeed(&tty,B115200);
		tty.c_iflag &= ~IGNBRK;
		tty.c_lflag = 0;
		tty.c_oflag = 0;
		tty.c_cc[VMIN] = 0;
		tty.c_cc[VTIME] = 5;
		tty.c_iflag &= ~(IXON | IXOFF | IXANY);
		tty.c_cflag |= CLOCAL | CREAD | CS8 | PARENB;
		tty.c_cflag &= ~(PARENB | PARODD | CRTSCTS | CSIZE | CSTOPB);
		tcsetattr(pfd,TCSANOW,&tty);
	}
	else
		printf("Cannot open serial port for head servos (port \"%s\", %s), so no head motion available\n",headname,strerror(errno));

	// open body servo connection
	dfd = open(bodyname,O_RDWR | O_NOCTTY | O_SYNC);
	if(dfd != -1)
	{
		struct termios tty;
		memset(&tty,0,sizeof(struct termios));
		tcgetattr(dfd,&tty);
		cfsetospeed(&tty,B115200);
		cfsetispeed(&tty,B115200);
		tty.c_iflag &= ~IGNBRK;
		tty.c_lflag = 0;
		tty.c_oflag = 0;
		tty.c_cc[VMIN] = 0;
		tty.c_cc[VTIME] = 5;
		tty.c_iflag &= ~(IXON | IXOFF | IXANY);
		tty.c_cflag |= CLOCAL | CREAD | CS8 | PARENB;
		tty.c_cflag &= ~(PARENB | PARODD | CRTSCTS | CSIZE | CSTOPB);
		tcsetattr(dfd,TCSANOW,&tty);
	}
	else
		printf("Cannot open serial port for body servos (port \"%s\", %s), so no body motion available\n",bodyname,strerror(errno));

	Reset();

	random_head_yaw = 0.0f;
	random_head_pitch = 0.0f;
	random_head_roll = 0.0f;
	random_eyes_yaw = 0.0f;
	random_eyes_pitch = 0.0f;
	random_face_brows = 0.0f;
	random_face_smile = 0.0f;

	// build gesture masters
	master[oz::GESTURE_BROW_RAISE_LONG].tracks = 1;
	master[oz::GESTURE_BROW_RAISE_LONG].track[0].id = oz::SERVO_BROWS;
	master[oz::GESTURE_BROW_RAISE_LONG].track[0].slots = 2;
	master[oz::GESTURE_BROW_RAISE_LONG].track[0].slot[0].ts = 0;
	master[oz::GESTURE_BROW_RAISE_LONG].track[0].slot[0].v = 1.0f;
	master[oz::GESTURE_BROW_RAISE_LONG].track[0].slot[1].ts = 1000;
	master[oz::GESTURE_BROW_RAISE_LONG].track[0].slot[1].v = 0.0f;

	master[oz::GESTURE_BROW_RAISE_SHORT].tracks = 1;
	master[oz::GESTURE_BROW_RAISE_SHORT].track[0].id = oz::SERVO_BROWS;
	master[oz::GESTURE_BROW_RAISE_SHORT].track[0].slots = 2;
	master[oz::GESTURE_BROW_RAISE_SHORT].track[0].slot[0].ts = 0;
	master[oz::GESTURE_BROW_RAISE_SHORT].track[0].slot[0].v = 1.0f;
	master[oz::GESTURE_BROW_RAISE_SHORT].track[0].slot[1].ts = 250;
	master[oz::GESTURE_BROW_RAISE_SHORT].track[0].slot[1].v = 0.0f;

	master[oz::GESTURE_FROWN_LONG].tracks = 1;
	master[oz::GESTURE_FROWN_LONG].track[0].id = oz::SERVO_BROWS;
	master[oz::GESTURE_FROWN_LONG].track[0].slots = 2;
	master[oz::GESTURE_FROWN_LONG].track[0].slot[0].ts = 0;
	master[oz::GESTURE_FROWN_LONG].track[0].slot[0].v = -1.0f;
	master[oz::GESTURE_FROWN_LONG].track[0].slot[1].ts = 1000;
	master[oz::GESTURE_FROWN_LONG].track[0].slot[1].v = 0.0f;

	master[oz::GESTURE_FROWN_SHORT].tracks = 1;
	master[oz::GESTURE_FROWN_SHORT].track[0].id = oz::SERVO_BROWS;
	master[oz::GESTURE_FROWN_SHORT].track[0].slots = 2;
	master[oz::GESTURE_FROWN_SHORT].track[0].slot[0].ts = 0;
	master[oz::GESTURE_FROWN_SHORT].track[0].slot[0].v = -1.0f;
	master[oz::GESTURE_FROWN_SHORT].track[0].slot[1].ts = 250;
	master[oz::GESTURE_FROWN_SHORT].track[0].slot[1].v = 0.0f;

	master[oz::GESTURE_YES_LONG].tracks = 1;
	master[oz::GESTURE_YES_LONG].track[0].id = oz::SERVO_NECK_PITCH;
	master[oz::GESTURE_YES_LONG].track[0].slots = 2;
	master[oz::GESTURE_YES_LONG].track[0].slot[0].ts = 0;
	master[oz::GESTURE_YES_LONG].track[0].slot[0].v = -1.0f;
	master[oz::GESTURE_YES_LONG].track[0].slot[1].ts = 1000;
	master[oz::GESTURE_YES_LONG].track[0].slot[1].v = 0.0f;

	master[oz::GESTURE_YES_SHORT].tracks = 1;
	master[oz::GESTURE_YES_SHORT].track[0].id = oz::SERVO_NECK_PITCH;
	master[oz::GESTURE_YES_SHORT].track[0].slots = 2;
	master[oz::GESTURE_YES_SHORT].track[0].slot[0].ts = 0;
	master[oz::GESTURE_YES_SHORT].track[0].slot[0].v = -1.0f;
	master[oz::GESTURE_YES_SHORT].track[0].slot[1].ts = 250;
	master[oz::GESTURE_YES_SHORT].track[0].slot[1].v = 0.0f;

	master[oz::GESTURE_YES_LONG_3X].tracks = 1;
	master[oz::GESTURE_YES_LONG_3X].track[0].id = oz::SERVO_NECK_PITCH;
	master[oz::GESTURE_YES_LONG_3X].track[0].slots = 6;
	master[oz::GESTURE_YES_LONG_3X].track[0].slot[0].ts = 0;
	master[oz::GESTURE_YES_LONG_3X].track[0].slot[0].v = -1.0f;
	master[oz::GESTURE_YES_LONG_3X].track[0].slot[1].ts = 1000;
	master[oz::GESTURE_YES_LONG_3X].track[0].slot[1].v = 1.0f;
	master[oz::GESTURE_YES_LONG_3X].track[0].slot[2].ts = 2000;
	master[oz::GESTURE_YES_LONG_3X].track[0].slot[2].v = -1.0f;
	master[oz::GESTURE_YES_LONG_3X].track[0].slot[3].ts = 3000;
	master[oz::GESTURE_YES_LONG_3X].track[0].slot[3].v = 1.0f;
	master[oz::GESTURE_YES_LONG_3X].track[0].slot[4].ts = 4000;
	master[oz::GESTURE_YES_LONG_3X].track[0].slot[4].v = -1.0f;
	master[oz::GESTURE_YES_LONG_3X].track[0].slot[5].ts = 5000;
	master[oz::GESTURE_YES_LONG_3X].track[0].slot[5].v = 0.0f;

	master[oz::GESTURE_YES_SHORT_3X].tracks = 1;
	master[oz::GESTURE_YES_SHORT_3X].track[0].id = oz::SERVO_NECK_PITCH;
	master[oz::GESTURE_YES_SHORT_3X].track[0].slots = 6;
	master[oz::GESTURE_YES_SHORT_3X].track[0].slot[0].ts = 0;
	master[oz::GESTURE_YES_SHORT_3X].track[0].slot[0].v = -1.0f;
	master[oz::GESTURE_YES_SHORT_3X].track[0].slot[1].ts = 250;
	master[oz::GESTURE_YES_SHORT_3X].track[0].slot[1].v = 1.0f;
	master[oz::GESTURE_YES_SHORT_3X].track[0].slot[2].ts = 500;
	master[oz::GESTURE_YES_SHORT_3X].track[0].slot[2].v = -1.0f;
	master[oz::GESTURE_YES_SHORT_3X].track[0].slot[3].ts = 750;
	master[oz::GESTURE_YES_SHORT_3X].track[0].slot[3].v = 1.0f;
	master[oz::GESTURE_YES_SHORT_3X].track[0].slot[4].ts = 1000;
	master[oz::GESTURE_YES_SHORT_3X].track[0].slot[4].v = -1.0f;
	master[oz::GESTURE_YES_SHORT_3X].track[0].slot[5].ts = 1250;
	master[oz::GESTURE_YES_SHORT_3X].track[0].slot[5].v = 0.0f;

	master[oz::GESTURE_NO_LONG].tracks = 1;
	master[oz::GESTURE_NO_LONG].track[0].id = oz::SERVO_NECK_YAW;
	master[oz::GESTURE_NO_LONG].track[0].slots = 3;
	master[oz::GESTURE_NO_LONG].track[0].slot[0].ts = 0;
	master[oz::GESTURE_NO_LONG].track[0].slot[0].v = -0.3f;
	master[oz::GESTURE_NO_LONG].track[0].slot[1].ts = 1000;
	master[oz::GESTURE_NO_LONG].track[0].slot[1].v = 0.3f;
	master[oz::GESTURE_NO_LONG].track[0].slot[2].ts = 2000;
	master[oz::GESTURE_NO_LONG].track[0].slot[2].v = 0.0f;

	master[oz::GESTURE_NO_SHORT].tracks = 1;
	master[oz::GESTURE_NO_SHORT].track[0].id = oz::SERVO_NECK_YAW;
	master[oz::GESTURE_NO_SHORT].track[0].slots = 3;
	master[oz::GESTURE_NO_SHORT].track[0].slot[0].ts = 0;
	master[oz::GESTURE_NO_SHORT].track[0].slot[0].v = -0.3f;
	master[oz::GESTURE_NO_SHORT].track[0].slot[1].ts = 250;
	master[oz::GESTURE_NO_SHORT].track[0].slot[1].v = 0.3f;
	master[oz::GESTURE_NO_SHORT].track[0].slot[2].ts = 500;
	master[oz::GESTURE_NO_SHORT].track[0].slot[2].v = 0.0f;

	master[oz::GESTURE_NO_LONG_3X].tracks = 1;
	master[oz::GESTURE_NO_LONG_3X].track[0].id = oz::SERVO_NECK_YAW;
	master[oz::GESTURE_NO_LONG_3X].track[0].slots = 6;
	master[oz::GESTURE_NO_LONG_3X].track[0].slot[0].ts = 0;
	master[oz::GESTURE_NO_LONG_3X].track[0].slot[0].v = -0.3f;
	master[oz::GESTURE_NO_LONG_3X].track[0].slot[1].ts = 1000;
	master[oz::GESTURE_NO_LONG_3X].track[0].slot[1].v = 0.3f;
	master[oz::GESTURE_NO_LONG_3X].track[0].slot[2].ts = 2000;
	master[oz::GESTURE_NO_LONG_3X].track[0].slot[2].v = -0.3f;
	master[oz::GESTURE_NO_LONG_3X].track[0].slot[3].ts = 3000;
	master[oz::GESTURE_NO_LONG_3X].track[0].slot[3].v = 0.3f;
	master[oz::GESTURE_NO_LONG_3X].track[0].slot[4].ts = 4000;
	master[oz::GESTURE_NO_LONG_3X].track[0].slot[4].v = -0.3f;
	master[oz::GESTURE_NO_LONG_3X].track[0].slot[5].ts = 5000;
	master[oz::GESTURE_NO_LONG_3X].track[0].slot[5].v = 0.0f;

	master[oz::GESTURE_NO_SHORT_3X].tracks = 1;
	master[oz::GESTURE_NO_SHORT_3X].track[0].id = oz::SERVO_NECK_YAW;
	master[oz::GESTURE_NO_SHORT_3X].track[0].slots = 6;
	master[oz::GESTURE_NO_SHORT_3X].track[0].slot[0].ts = 0;
	master[oz::GESTURE_NO_SHORT_3X].track[0].slot[0].v = -0.3f;
	master[oz::GESTURE_NO_SHORT_3X].track[0].slot[1].ts = 250;
	master[oz::GESTURE_NO_SHORT_3X].track[0].slot[1].v = 0.3f;
	master[oz::GESTURE_NO_SHORT_3X].track[0].slot[2].ts = 500;
	master[oz::GESTURE_NO_SHORT_3X].track[0].slot[2].v = -0.3f;
	master[oz::GESTURE_NO_SHORT_3X].track[0].slot[3].ts = 750;
	master[oz::GESTURE_NO_SHORT_3X].track[0].slot[3].v = 0.3f;
	master[oz::GESTURE_NO_SHORT_3X].track[0].slot[4].ts = 1000;
	master[oz::GESTURE_NO_SHORT_3X].track[0].slot[4].v = -0.3f;
	master[oz::GESTURE_NO_SHORT_3X].track[0].slot[5].ts = 1250;
	master[oz::GESTURE_NO_SHORT_3X].track[0].slot[5].v = 0.0f;

	master[oz::GESTURE_INDIAN_WOBBLE].tracks = 1;
	master[oz::GESTURE_INDIAN_WOBBLE].track[0].id = oz::SERVO_NECK_ROLL;
	master[oz::GESTURE_INDIAN_WOBBLE].track[0].slots = 6;
	master[oz::GESTURE_INDIAN_WOBBLE].track[0].slot[0].ts = 0;
	master[oz::GESTURE_INDIAN_WOBBLE].track[0].slot[0].v = -0.25f;
	master[oz::GESTURE_INDIAN_WOBBLE].track[0].slot[1].ts = 250;
	master[oz::GESTURE_INDIAN_WOBBLE].track[0].slot[1].v = 0.25f;
	master[oz::GESTURE_INDIAN_WOBBLE].track[0].slot[2].ts = 500;
	master[oz::GESTURE_INDIAN_WOBBLE].track[0].slot[2].v = -0.25f;
	master[oz::GESTURE_INDIAN_WOBBLE].track[0].slot[3].ts = 750;
	master[oz::GESTURE_INDIAN_WOBBLE].track[0].slot[3].v = 0.25f;
	master[oz::GESTURE_INDIAN_WOBBLE].track[0].slot[4].ts = 1000;
	master[oz::GESTURE_INDIAN_WOBBLE].track[0].slot[4].v = -0.25f;
	master[oz::GESTURE_INDIAN_WOBBLE].track[0].slot[5].ts = 1250;
	master[oz::GESTURE_INDIAN_WOBBLE].track[0].slot[5].v = 0.0f;

	printf("Initialized servos\n");
}


Servos::~Servos()
{
	if(pfd != -1)
		close(pfd);
	if(dfd != -1)
		close(dfd);
}


void Servos::Step()
{
	// prepare poll structures
	struct pollfd pfd[2];

	pfd[0].fd = rfd;
	pfd[0].events = POLLIN;
	pfd[0].revents = 0;

	pfd[1].fd = tfd;
	pfd[1].events = POLLIN;
	pfd[1].revents = 0;

	poll(pfd,2,-1);

	if(pfd[0].revents & POLLIN)
		FlushMessages();

	if(pfd[1].revents & POLLIN)
	{
		unsigned char buffer[8];
		read(tfd,buffer,8);

		// calculate base expression
		float expression_brows,expression_smile;
		switch(expression)
		{
			case oz::EXPRESSION_NEUTRAL:
				expression_brows = 0.0f;
				expression_smile = 0.0f;
				break;

			case oz::EXPRESSION_HAPPY:
				expression_brows = 1.0f;
				expression_smile = 1.0f;
				break;

			case oz::EXPRESSION_SAD:
				expression_brows = -1.0f;
				expression_smile = -1.0f;
				break;

			case oz::EXPRESSION_EVIL:
				expression_brows = -1.0f;
				expression_smile = 1.0f;
				break;

			case oz::EXPRESSION_AFRAID:
				expression_brows = 1.0f;
				expression_smile = -1.0f;
				break;
		}

		// calculate base random head motion
		if((rand() % 100) > (100 - RANDOM_HEAD_CHANCE))
		{
			random_head_yaw = -1.0f + 2.0f * (float)(rand() % 100000) / 99999.0f;
			random_head_pitch = -1.0f + 2.0f * (float)(rand() % 100000) / 99999.0f;
			random_head_roll = -1.0f + 2.0f * (float)(rand() % 100000) / 99999.0f;
		}

		// calculate base random eyes motion
		if((rand() % 100) > (100 - RANDOM_EYES_CHANCE))
		{
			random_eyes_yaw = -1.0f + 2.0f * (float)(rand() % 100000) / 99999.0f;
			random_eyes_pitch = -1.0f + 2.0f * (float)(rand() % 100000) / 99999.0f;
		}

		// calculate base random face motion
		if((rand() % 100) > (100 - RANDOM_FACE_CHANCE))
		{
			random_face_brows = -1.0f + 2.0f * (float)(rand() % 100000) / 99999.0f;
			random_face_smile = -1.0f + 2.0f * (float)(rand() % 100000) / 99999.0f;
		}

		// process blinking
		bool blinking = false;
		bc += (float)SERVOS_MSEC * 0.001f;
		if(bc >= 1.0f / auto_blink_speed)
		{
			bc = 0.0f;
			blinking = true;
		}

		// reset servo positions
		float pos[oz::MAX_SERVOS];
		memset(pos,0,oz::MAX_SERVOS * sizeof(float));

		// get current timestamp
		struct timeval tv;
		gettimeofday(&tv,0);
		unsigned long long ts = (tv.tv_sec * 1000) + ((tv.tv_usec / 1000) % 1000);

		// add running gestures
		for(int i = 0; i < gestures; i++)
			if(!AddGesture(pos,ts,&(gesture[i])))
			{
				// animation has passed, free it
				memcpy(&(gesture[i]),&(gesture[i + 1]),(gestures - i - 1) * sizeof(gesture_instance));
				gestures--;
				i--;
			}

		// add lipsync 
		if(lipsync)
			if(!AddLipsync(pos,ts))
			{
				lipsync->release();
				lipsync = 0;
			}

		// add special head servos
		pos[oz::SERVO_NECK_YAW] += 0.5f + RANDOM_HEAD_YAW_RANGE * 0.5f * random_head_yaw * random_head_weight;
		pos[oz::SERVO_NECK_ROLL] += 0.5f + RANDOM_HEAD_ROLL_RANGE * 0.5f * random_head_roll * random_head_weight;
		pos[oz::SERVO_NECK_PITCH] += 0.5f + RANDOM_HEAD_PITCH_RANGE * 0.5f * random_head_pitch * random_head_weight;
		pos[oz::SERVO_BROWS] += 0.5f + EXPRESSION_BROWS_RANGE * 0.5f * expression_brows * expression_weight + RANDOM_FACE_BROWS_RANGE * 0.5f * random_face_brows * random_face_weight;
		if(blinking)
			pos[oz::SERVO_EYELIDS] += 0.0f;
		else
			pos[oz::SERVO_EYELIDS] += 1.0f;
		pos[oz::SERVO_EYES_PITCH] += 0.5f + RANDOM_EYES_PITCH_RANGE * 0.5f * random_eyes_pitch * random_eyes_weight;
		pos[oz::SERVO_EYE_YAW_RIGHT] += 0.5f + RANDOM_EYES_YAW_RANGE * 0.5f * random_eyes_yaw * random_eyes_weight;
		pos[oz::SERVO_SMILE_LEFT] += 0.5f + EXPRESSION_SMILE_RANGE * 0.5f * expression_smile * expression_weight + RANDOM_FACE_SMILE_RANGE * 0.5f * random_face_smile * random_face_weight;
		pos[oz::SERVO_EYE_YAW_LEFT] += 0.5f + RANDOM_EYES_YAW_RANGE * 0.5f * random_eyes_yaw * random_eyes_weight;
		pos[oz::SERVO_SMILE_RIGHT] += 0.5f + EXPRESSION_SMILE_RANGE * 0.5f * expression_smile * expression_weight + RANDOM_FACE_SMILE_RANGE * 0.5f * random_face_smile * random_face_weight;

		// clamp to extremes
		for(int i = 0; i < oz::MAX_SERVOS; i++)
		{
			if(pos[i] < 0.0f)
				pos[i] = 0.0f;
			if(pos[i] > 1.0f)
				pos[i] = 1.0f;
		}

		// convert to integers
		int intpos[oz::MAX_SERVOS];
		for(int i = 0; i < oz::MAX_SERVOS; i++)
			intpos[i] = limits[i].lo + (int)(pos[i] * (float)(limits[i].hi - limits[i].lo));

		// update servo controllers
		for(int i = 0; i < 21; i++)
			SetBodyServo(limits[i].id,intpos[i]);
		for(int i = 0; i < 11; i++)
			SetHeadServo(limits[21 + i].id,intpos[21 + i]);
		for(int i = 0; i < 4; i++)
			SetHandServo(limits[32 + i].id,intpos[32 + i]);
	}
}


void Servos::Message(int msg,void* data)
{
	switch(msg)
	{
		case ITM_RESET:
			Reset();
			break;

		case ITM_GESTURE:
			if(gestures < MAX_INSTANCES)
			{
				int id = *(unsigned int*)&data;
				struct timeval tv;
				gettimeofday(&tv,0);
				gesture[gestures].gt = &(master[id]);
				gesture[gestures].ts = (tv.tv_sec * 1000) + ((tv.tv_usec / 1000) % 1000);
				gestures++;
			}
			break;

		case ITM_GESTURE_WEIGHT:
			gesture_weight = *(float*)&data;
			break;

		case ITM_EXPRESSION:
			expression = *(unsigned int*)&data;
			break;

		case ITM_EXPRESSION_WEIGHT:
			expression_weight = *(float*)&data;
			break;

		case ITM_AUTO_BLINK_SPEED:
			auto_blink_speed = *(float*)&data;
			break;

		case ITM_RANDOM_HEAD_WEIGHT:
			random_head_weight = *(float*)&data;
			break;

		case ITM_RANDOM_EYES_WEIGHT:
			random_eyes_weight = *(float*)&data;
			break;

		case ITM_RANDOM_FACE_WEIGHT:
			random_face_weight = *(float*)&data;
			break;

		case ITM_SERVO:
			// TODO
			break;

		case ITM_ALL_SERVOS:
			// TODO
			break;

		case ITM_DIRECT_WEIGHT:
			direct_weight = *(float*)&data;
			printf("direct_weight %f\n",direct_weight);
			break;

		case ITM_LOOKAT:
			lookat = *(unsigned int*)&data;
			printf("lookat %d\n",lookat);
			break;

		case ITM_LOOKAT_WEIGHT:
			lookat_weight = *(float*)&data;
			printf("lookat_weight %f\n",lookat_weight);
			break;

		case ITM_LIPSYNC:
			if(lipsync)
				lipsync->release();
			lipsync = (lipsyncbuffer*)data;
			break;
	}
}


void Servos::Reset()
{
	// home all servos
	for(int i = 0; i < 21; i++)
		SetBodyServo(i,limits[i].home);
	for(int i = 0; i < 11; i++)
		SetHeadServo(i,limits[i + 21].home);
	for(int i = 0; i < 4; i++)
		SetHandServo(i,limits[i + 32].home);

	// reset default settings
	gestures = 0;
	bc = 0;
	expression = oz::EXPRESSION_NEUTRAL;
	expression_weight = 1.0f;
	gesture_weight = 1.0f;
	auto_blink_speed = 0.1f;
	random_head_weight = 0.0f;
	random_eyes_weight = 0.0f;
	random_face_weight = 0.0f;
	direct_weight = 0.0f;
	lookat = oz::LOOKAT_FORWARD;
	lookat_weight = 1.0f;
}


void Servos::SetHeadServo(int id,int pos)
{
	if(pfd != -1)
	{
		pos *= 4;
		unsigned char buffer[] = { 0xAA,0x0C,0x04,(unsigned char)id,(unsigned char)(pos & 0x7F),(unsigned char)((pos >> 7) & 0x7F) };
		int bytes = write(pfd,buffer,sizeof(buffer));
		if(bytes != sizeof(buffer))
			printf("write error in SetHeadServo\n");
	}
}


int Servos::GetHeadServo(int id)
{
	if(pfd != -1)
	{
		unsigned char buffer[] = { 0xAA,0x0C,0x10,(unsigned char)id };
		int bytes = write(pfd,buffer,sizeof(buffer));
		if(bytes != sizeof(buffer))
		{
			printf("write error in GetHeadServo\n");
			return -1;
		}
		bytes = read(pfd,buffer,2);
		if(bytes != 2)
		{
			printf("read error in GetHeadServo\n");
			return -1;
		}
		return buffer[0] | (buffer[1] << 8);
	}
	return 0;
}


void Servos::SetHeadServos(int id,int n,int* pos)
{
	if(pfd != -1)
	{
		unsigned char buffer[256];
		buffer[0] = 0xAA;
		buffer[1] = 0x0C;
		buffer[2] = 0x1F;
		buffer[3] = n;
		buffer[4] = id;
		for(int i = 0; i < n; i++)
		{
			int p = pos[i] * 4;
			buffer[5 + i * 2] = p & 0x7F;
			buffer[5 + i * 2 + 1] = (p >> 7) & 0x7F;
		}
		int bytes = write(pfd,buffer,5 + 2 * n);
		if(bytes != 5 + 2 * n)
			printf("write error in SetHeadServos\n");
	}
}


void Servos::SetBodyServo(int id,int pos)
{
}


int Servos::GetBodyServo(int id)
{
	return 0;
}


void Servos::SetBodyServos(int id,int n,int* pos)
{
}


void Servos::SetHandServo(int id,int pos)
{
}


int Servos::GetHandServo(int id)
{
	return 0;
}


void Servos::SetHandServos(int id,int n,int* pos)
{
}


bool Servos::AddGesture(float* pos,unsigned long long ts,gesture_instance* inst)
{
	// if gesture does not start yet, ignore
	if(inst->ts > ts)
		return true;

	// if invalid gesture, indicate it should be removed and return
	if(!(inst->gt))
		return false;

	// find last ts
	unsigned long long last_ts = 0;
	for(int i = 0; i < inst->gt->tracks; i++)
		if(inst->gt->track[i].slots > 0)
			if(inst->ts + inst->gt->track[i].slot[inst->gt->track[i].slots - 1].ts > last_ts)
				last_ts = inst->ts + inst->gt->track[i].slot[inst->gt->track[i].slots - 1].ts;

	// if gesture already finished, indicate it should be removed and return
	if(ts >= last_ts)
		return false;

	// go over all tracks
	for(int i = 0; i < inst->gt->tracks; i++)
	{
		// ignore empty track
		if(inst->gt->track[i].slots == 0)
			continue;

		// sample the track
		int id = inst->gt->track[i].id;
		int k;
		for(k = 0; k < inst->gt->track[i].slots; k++)
			if(ts < inst->ts + inst->gt->track[i].slot[k].ts)
				break;
		if(k > 0)
		{
			printf("%llu: %llu: %d: %f\n",ts,inst->ts + inst->gt->track[i].slot[k - 1].ts,k,inst->gt->track[i].slot[k - 1].v);
			pos[id] += gesture_weight * inst->gt->track[i].slot[k - 1].v;
		}
	}

	// if all tracks are done, return false
	return ts < last_ts;
}


bool Servos::AddLipsync(float* pos,unsigned long long ts)
{
	// if lipsync is not due yet, ignore
	unsigned long long first_ts = 0;
	for(int i = 0; i < lipsync->slots; i++)
		if((first_ts == 0) || (lipsync->slot[i].ts < first_ts))
			first_ts = lipsync->slot[i].ts;
	if(first_ts > ts)
		return true;

	if(lipsync->slots == 0)
		return false;

	if(ts >= lipsync->slot[lipsync->slots - 1].ts)
		return false;

	int i;
	for(i = 0; i < lipsync->slots; i++)
		if(ts < lipsync->slot[i].ts)
			break;
	pos[oz::SERVO_JAW] += lipsync->slot[i].v;

	return true;
}
