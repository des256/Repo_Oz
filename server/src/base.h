// OZ - Robot Puppeteering Server
// (C) Copyrights by Desmond Germans
// 2014 Germans Media Technology & Services

#ifndef _BASE_H_
#define _BASE_H_

// base.h contains the conditional defines to include/exclude parts of the
// code, as well as all system includes, and the internal message
// definitions.


// Acapela TTS
#define USE_ACAPELA_TTS

// Ivona TTS
//#define USE_IVONA_TTS

// Google STT
#define USE_GOOGLE_STT

// OpenCV face recognition
//#define USE_OPENCV_VISION

// video size
#define VIDEO_XSIZE 640
#define VIDEO_YSIZE 480

// size of a complete YUV420 video frame
#define VIDEO_FRAME ((VIDEO_XSIZE * VIDEO_YSIZE * 3) / 2)

// video frame rate
#define VIDEO_FPS 30

// directories and configuration settings
#define DATADIR "../../../data/"

#ifdef USE_ACAPELA_TTS
#define ACAPELA_LICENSE \
	"\"497 0 WDAP #EVALUATION#Germans Media Technology Services     \"\n" \
	"Uq5Zkbp7R2XIWUGdBPbSdCGceF@pqAnT4u8xUNBdW%WIh!P2t7WFLtHTeFR!QjjBpcDe@ZkN$Tbq2JJPaAf#\n" \
	"RG$cnNHBqu6ZgAsKEuPXW2pnei3xcqmjUSRzyZgcqj3@ryGI\n" \
	"W2WKqR5kFR8S$UF283n9MQ##\n"
#define ACAPELA_USERID 0x50414457
#define ACAPELA_PASSWORD 0x00212B1D
#define ACAPELA_VOICE (char*)(DATADIR "femke/dun_femke_22k_ns.bvcu.ini")
#endif

#ifdef USE_IVONA_TTS
#define IVONA_CERTIFICATE_FILE (char*)(DATADIR "ivona-trial.ca")
#define IVONA_VOICE_LIB (char*)(DATADIR "libvoice_nl_lotte.so")
#define IVONA_VOICE_DB (char*)(DATADIR "vox_nl_lotte22i")
#endif

#ifdef USE_OPENCV_VISION
#define FACE_CASCADE_FILE (char*)(DATADIR "haarcascade_frontalface_default.xml")
#endif

#define MICROPHONE_DEVICE ((char*)"default")

#define SPEAKER_DEVICE ((char*)"default")

#define CAMERA_DEVICE ((char*)"/dev/video1")

#define HEAD_PORTNAME ((char*)"/dev/ttyACM0")

#define BODY_PORTNAME ((char*)"/dev/ttyUSB3")

#define API_VERSION 0x01000000

#define ROBOT_NAME ((char*)"Alice")

#define ROBOT_LONG_NAME ((char*)"Robokind R-50 (Alice)")

// includes
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <math.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/eventfd.h>
#include <sys/timerfd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define ALSA_PCM_NEW_HW_PARAMS_API
#include <alsa/asoundlib.h>
#include <linux/videodev2.h>
#include <jpeglib.h>

#ifdef USE_ACAPELA_TTS
extern "C" {
#define PLATFORM_LINUX
#include <babile/i_babile.h>
}
#endif

#ifdef USE_IVONA_TTS
#include <ivona_tts.h>
#endif

#ifdef USE_GOOGLE_STT
#include "FLAC/stream_encoder.h"
#include <curl/curl.h>
#include <json/json.h>
#endif

#ifdef USE_OPENCV_VISION
#include <opencv2/core/core.hpp>
#include <opencv2/contrib/contrib.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/contrib/detection_based_tracker.hpp>
#endif


enum
{
	ITM_NOTHING = 0,         // illegal message, data = null
	ITM_EXIT,                // gracefully terminate thread, data = null
	ITM_TEXT,                // server->tts, convert text string to speech sample, data = text string
	ITM_SPEECH_DONE,         // tts->server, text speech is done, data = null
	ITM_GESTURE_DONE,        // servos->server, gesture is done, data = null
	ITM_AUDIO,               // microphone->stt, start analyzing a new sample, data = audio buffer
	ITM_SPEECH,              // tts->speaker, append sample to audio output, data = speech buffer
	ITM_VIDEO,               // camera->server, a new frame is ready, data = video buffer
					         // server->vision, indicate to the vision task that a new frame is ready, data = video buffer
	ITM_FACES,               // vision->server, face recognition updates, data = face buffer
	ITM_UTTERANCE,           // stt->server, utterance found, data = allocated std::map<float,std::string> with the utterance data, data = utterance buffer
	ITM_MUTE,                // tts->microphone, mute when speaking, data = null
	ITM_UNMUTE,              // tts->microphone, unmute after speaking, data = null
	ITM_RESET,               // server->servos, reset servo state, data = null
	ITM_GESTURE,             // server->servos, start gesture, data = gesture number
							 // or tts->servos, started gesture from speech, data = gesture number
							 // or tts->server, started gesture from speech, data = gesture number
	ITM_GESTURE_WEIGHT,      // server->servos, set gesture weight, data = gesture weight
	ITM_EXPRESSION,          // server->servos, set expression, data = expression number
	ITM_EXPRESSION_WEIGHT,   // server->servos, set expression weight, data = expression weight
	ITM_AUTO_BLINK_SPEED,    // server->servos, set auto blink speed, data = auto blink speed
	ITM_RANDOM_HEAD_WEIGHT,  // server->servos, set random head weight, data = weight
	ITM_RANDOM_EYES_WEIGHT,  // server->servos, set random eyes weight, data = weight
	ITM_RANDOM_FACE_WEIGHT,  // server->servos, set random face weight, data = weight
	ITM_SERVO,               // server->servos, set servo position, TBD
	ITM_ALL_SERVOS,          // server->servos, set all servo positions, TBD
	ITM_DIRECT_WEIGHT,       // server->servos, set direct weight, data = weight
	ITM_LOOKAT,              // server->servos, set lookat, data = lookat number
	ITM_LOOKAT_WEIGHT,       // server->servos, set lookat weight, data = weight
	ITM_LIPSYNC,             // tts->servos, start lipsync animation, data = gesture buffer
};


struct int2
{
	int x,y;
	int2(int ax = 0,int ay = 0) : x(ax),y(ay) { };
};


struct intr
{
	int2 o,s;
	intr(const int2& ao = int2(0,0),const int2& as = int2(0,0)) : o(ao),s(as) { };
};


struct float2
{
	float x,y;
	float2(float ax = 0,float ay = 0) : x(ax),y(ay) { };
};


struct floatr
{
	float2 o,s;
	floatr(const float2& ao = float2(0.0f,0.0f),const float2& as = float2(0.0f,0.0f)) : o(ao),s(as) { };
};


// global clock start
extern unsigned long long global_start_usec;


#endif  // _BASE_H_
