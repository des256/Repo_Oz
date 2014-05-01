// OZ - Robot Puppeteering System Client Protocol, up to version 1.0
// (C) Copyrights by Desmond Germans
// 2014 Germans Media Technology & Services

#ifndef _OZ_PROTOCOL_H_
#define _OZ_PROTOCOL_H_


namespace oz
{


// the port
enum
{
	OZ_PORT = 7331
};


// servo ids
enum
{
    // body
    SERVO_WAIST = 0,

    // left arm
    SERVO_SHOULDER_PITCH_LEFT,
    SERVO_SHOULDER_ROLL_LEFT,
    SERVO_ELBOW_YAW_LEFT,
    SERVO_ELBOW_PITCH_LEFT,

    // right arm
    SERVO_SHOULDER_PITCH_RIGHT,
    SERVO_SHOULDER_ROLL_RIGHT,
    SERVO_ELBOW_YAW_RIGHT,
    SERVO_ELBOW_PITCH_RIGHT,

    // left leg
    SERVO_HIP_ROLL_LEFT,
    SERVO_HIP_YAW_LEFT,
    SERVO_HIP_PITCH_LEFT,
    SERVO_KNEE_PITCH_LEFT,
    SERVO_ANKLE_PITCH_LEFT,
    SERVO_ANKLE_ROLL_LEFT,

    // right leg
    SERVO_HIP_ROLL_RIGHT,
    SERVO_HIP_YAW_RIGHT,
    SERVO_HIP_PITCH_RIGHT,
    SERVO_KNEE_PITCH_RIGHT,
    SERVO_ANKLE_PITCH_RIGHT,
    SERVO_ANKLE_ROLL_RIGHT,

    // head
    SERVO_NECK_YAW,
    SERVO_NECK_ROLL,
    SERVO_NECK_PITCH,
    SERVO_BROWS,
    SERVO_EYELIDS,
    SERVO_EYES_PITCH,
    SERVO_JAW,
    SERVO_EYE_YAW_RIGHT,
    SERVO_SMILE_LEFT,
    SERVO_EYE_YAW_LEFT,
    SERVO_SMILE_RIGHT,

    // left hand
    SERVO_WRIST_LEFT,
    SERVO_GRASP_LEFT,

    // right hand
    SERVO_WRIST_RIGHT,
    SERVO_GRASP_RIGHT,

    MAX_SERVOS  // last servo indicator, do not use
};


// voice/gender/language specifications
enum
{
    VGL_DEFAULT = 0
};


// gestures
enum
{
    GESTURE_BROW_RAISE_LONG = 0,
    GESTURE_BROW_RAISE_SHORT,
    GESTURE_FROWN_LONG,
    GESTURE_FROWN_SHORT,
    GESTURE_YES_LONG,
    GESTURE_YES_SHORT,
    GESTURE_YES_LONG_3X,
    GESTURE_YES_SHORT_3X,
    GESTURE_NO_LONG,
    GESTURE_NO_SHORT,
    GESTURE_NO_LONG_3X,
    GESTURE_NO_SHORT_3X,
    GESTURE_INDIAN_WOBBLE,

    MAX_GESTURES
};


// facial expressions
enum
{
    EXPRESSION_NEUTRAL = 0,
    EXPRESSION_HAPPY,
    EXPRESSION_SAD,
    EXPRESSION_EVIL,
    EXPRESSION_AFRAID
};


// lookat targets
enum
{
    LOOKAT_FORWARD = 0,
    LOOKAT_DOWN,
    LOOKAT_UP,
    LOOKAT_LARGEST,
    LOOKAT_SMALLEST,
    LOOKAT_AWAY
};


enum
{
    MSG_NOTHING = 0,

    // admin
    MSG_REQUEST_VERSION,  // request particular version of the API, dword = version (0x01000000 = 1.0), robot returns version as answer, followed by name and long name
    MSG_RESET,            // reset robot

    // speaking
    MSG_SPEAK,                  // speak, text = utterance
    MSG_VOICE_GENDER_LANGUAGE,  // set voice/gender/language, dword = VCL_* (default = VCL_DEFAULT)
    MSG_VOICE_VOLUME,           // set voice volume, float = volume (0..1, default = 1)
    MSG_VOICE_PITCH,            // set voice pitch, float = pitch (0..2, default = 1)

    // gesturing
    MSG_GESTURE,         // manually trigger gesture, dword = GESTURE_*
    MSG_GESTURE_WEIGHT,  // set gesture weight, float = weight (0..1, default = 1)

    // facial expression
    MSG_EXPRESSION,         // set facial expression, dword = EXPRESSION_* (default = EXPRESSION_NEUTRAL)
    MSG_EXPRESSION_WEIGHT,  // set facial expression weight, float = weight (0..1, default = 1)

    // automatic motion
    MSG_AUTO_BLINK_SPEED,    // set automatic blink speed, float = speed in Hz (0..10, default = 0, no blinking)
    MSG_RANDOM_HEAD_WEIGHT,  // set random head motion weight, float = weight (0..1, default = 0)
    MSG_RANDOM_EYES_WEIGHT,  // set random eyes motion weight, float = weight (0..1, default = 0)
    MSG_RANDOM_FACE_WEIGHT,  // set random face motion weight, float = weight (0..1, default = 0)

    // direct servo control
    MSG_SERVO,          // set servo position, dword = id, float = position (0..1, default = 0)
    MSG_ALL_SERVOS,     // set all servo positions, { float = position (0..1, default = 0) }
    MSG_DIRECT_WEIGHT,  // set direct servo control weight, float = weight (0..1, default = 0)

    // looking at faces
    MSG_LOOKAT,         // set lookat target, dword = LOOKAT_* (default = LOOKAT_FORWARD)
    MSG_LOOKAT_WEIGHT,  // set lookat weight, float = weight (0..1, default = 0)

    // events from robot
    MSG_SPEAK_DONE,     // speak is done
    MSG_GESTURE_DONE,   // gesture is done
    MSG_SPEAK_GESTURE,  // speak-triggered gesture, dword = GESTURE_*
    MSG_UTTERANCE,      // utterance found, dword = count, { float = confidence, text = utterance }
    MSG_FACES,          // new face recognition state, dword = count, { float = confidence, float = x, float = y, float = xsize, float = ysize, text = identity }
    MSG_VIDEO_FRAME,    // single video frame from the robot, dword = length, data blob
};


}


#endif
