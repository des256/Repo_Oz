#include "QClient1.h"
#include "../protocol.h"
#include <QEventLoop>


QClient1::QClient1() : QObject(),socket(),is_connected(false),video_frame(0)
{
    initialize();
    video_frame = new unsigned char[1024 * 1024];
    connect(&socket,SIGNAL(connected()),this,SLOT(socketConnected()));
    connect(&socket,SIGNAL(disconnected()),this,SLOT(socketDisconnected()));
    connect(&socket,SIGNAL(readyRead()),this,SLOT(socketReadyRead()));
}


QClient1::~QClient1()
{
    delete[] video_frame;
}


void QClient1::connectToHost(const QString& hostName,quint16 port,QIODevice::OpenMode openMode,QAbstractSocket::NetworkLayerProtocol protocol)
{
    socket.connectToHost(hostName,port,openMode,protocol);
}


void QClient1::connectToHost(const QHostAddress& address,quint16 port,QIODevice::OpenMode openMode)
{
    socket.connectToHost(address,port,openMode);
}


bool QClient1::waitForConnected(int msecs)
{
    return socket.waitForConnected(msecs);
}


void QClient1::disconnectFromHost()
{
    socket.disconnectFromHost();
}


void QClient1::reset()
{
    if(is_connected)
    {
        sendDword(oz::MSG_RESET);
        initialize();
    }
}


QString QClient1::name()
{
    return short_name;
}


QString QClient1::longName()
{
    return long_name;
}


void QClient1::speak(QString& text)
{
    if(is_connected)
    {
        sendDword(oz::MSG_SPEAK);
        sendString(text.toUtf8().data());
        is_speaking = true;
        speakStart();
    }
}


void QClient1::waitForSpeak()
{
    if(is_connected)
    {
        QEventLoop loop;
        connect(this,SIGNAL(socketDisconnectedProxy()),&loop,SLOT(quit()));
        connect(this,SIGNAL(speakDoneProxy()),&loop,SLOT(quit()));
        loop.exec();
    }
}


bool QClient1::speaking()
{
    if(is_connected)
        return is_speaking;
    else
        return false;
}


void QClient1::setVoiceGenderLanguage(int vgl)
{
    if(is_connected)
    {
        sendDword(oz::MSG_VOICE_GENDER_LANGUAGE);
        sendDword(vgl);
        voice_gender_language = vgl;
    }
}


int QClient1::voiceGenderLanguage()
{
    if(is_connected)
        return voice_gender_language;
    else
        return oz::VGL_DEFAULT;
}


void QClient1::setVoiceVolume(float volume)
{
    if(is_connected)
    {
        sendDword(oz::MSG_VOICE_VOLUME);
        sendFloat(volume);
        voice_volume = volume;
    }
}


float QClient1::voiceVolume()
{
    if(is_connected)
        return voice_volume;
    else
        return 0.0f;
}


void QClient1::setVoicePitch(float pitch)
{
    if(is_connected)
    {
        sendDword(oz::MSG_VOICE_PITCH);
        sendFloat(pitch);
        voice_pitch = pitch;
    }
}


float QClient1::voicePitch()
{
    if(is_connected)
        return voice_pitch;
    else
        return 0.0f;
}


void QClient1::gesture(int id)
{
    if(is_connected)
    {
        sendDword(oz::MSG_GESTURE);
        sendDword(id);
        is_gesturing = true;
        gestureStart(id);
    }
}


void QClient1::waitForGesture()
{
    if(is_connected)
    {
        QEventLoop loop;
        connect(this,SIGNAL(socketDisconnectedProxy()),&loop,SLOT(quit()));
        connect(this,SIGNAL(gestureDoneProxy()),&loop,SLOT(quit()));
        loop.exec();
    }
}


bool QClient1::gesturing()
{
    if(is_connected)
        return is_gesturing;
    else
        return false;
}


void QClient1::setGestureWeight(float weight)
{
    if(is_connected)
    {
        sendDword(oz::MSG_GESTURE_WEIGHT);
        sendFloat(weight);
        gesture_weight = weight;
    }
}


float QClient1::gestureWeight()
{
    if(is_connected)
        return gesture_weight;
    else
        return 0.0f;
}


void QClient1::setExpression(int expr)
{
    if(is_connected)
    {
        sendDword(oz::MSG_EXPRESSION);
        sendDword(expr);
        current_expression = expr;
    }
}


int QClient1::expression()
{
    if(is_connected)
        return current_expression;
    else
        return oz::EXPRESSION_NEUTRAL;
}


void QClient1::setExpressionWeight(float weight)
{
    if(is_connected)
    {
        sendDword(oz::MSG_EXPRESSION_WEIGHT);
        sendFloat(weight);
        expression_weight = weight;
    }
}


float QClient1::expressionWeight()
{
    if(is_connected)
        return expression_weight;
    else
        return 0.0f;
}


void QClient1::setAutoBlinkSpeed(float speed)
{
    if(is_connected)
    {
        sendDword(oz::MSG_AUTO_BLINK_SPEED);
        sendFloat(speed);
        auto_blink_speed = speed;
    }
}


float QClient1::autoBlinkSpeed()
{
    if(is_connected)
        return auto_blink_speed;
    else
        return 0.0f;
}


void QClient1::setRandomHeadWeight(float weight)
{
    if(is_connected)
    {
        sendDword(oz::MSG_RANDOM_HEAD_WEIGHT);
        sendFloat(weight);
        random_head_weight = weight;
    }
}


float QClient1::randomHeadWeight()
{
    if(is_connected)
        return random_head_weight;
    else
        return 0.0f;
}


void QClient1::setRandomEyesWeight(float weight)
{
    if(is_connected)
    {
        sendDword(oz::MSG_RANDOM_EYES_WEIGHT);
        sendFloat(weight);
        random_eyes_weight = weight;
    }
}


float QClient1::randomEyesWeight()
{
    if(is_connected)
        return random_eyes_weight;
    else
        return 0.0f;
}


void QClient1::setRandomFaceWeight(float weight)
{
    if(is_connected)
    {
        sendDword(oz::MSG_RANDOM_FACE_WEIGHT);
        sendFloat(weight);
        random_face_weight = weight;
    }
}


float QClient1::randomFaceWeight()
{
    if(is_connected)
        return random_face_weight;
    else
        return 0.0f;
}


void QClient1::setServo(int id,float pos)
{
    if(is_connected)
    {
        sendDword(oz::MSG_SERVO);
        sendDword(id);
        sendFloat(pos);
        servo_pos[id] = pos;
    }
}


float QClient1::servo(int id)
{
    if(is_connected)
        return servo_pos[id];
    else
        return 0.0f;
}


void QClient1::setAllServos(float* pos)
{
    if(is_connected)
    {
        sendDword(oz::MSG_ALL_SERVOS);
        for(int i = 0; i < oz::MAX_SERVOS; i++)
            sendFloat(pos[i]);
        memcpy(servo_pos,pos,oz::MAX_SERVOS * sizeof(float));
    }
}


void QClient1::allServos(float* pos)
{
    if(is_connected)
        memcpy(pos,servo_pos,oz::MAX_SERVOS * sizeof(float));
}


void QClient1::setDirectWeight(float weight)
{
    if(is_connected)
    {
        sendDword(oz::MSG_DIRECT_WEIGHT);
        sendFloat(weight);
        direct_weight = weight;
    }
}


float QClient1::directWeight()
{
    if(is_connected)
        return direct_weight;
    else
        return 0.0f;
}


void QClient1::setLookAt(int target)
{
    if(is_connected)
    {
        sendDword(oz::MSG_LOOKAT);
        sendDword(target);
        current_lookat = target;
    }
}


int QClient1::lookAt()
{
    if(is_connected)
        return current_lookat;
    else
        return oz::LOOKAT_FORWARD;
}


void QClient1::setLookAtWeight(float weight)
{
    if(is_connected)
    {
        sendDword(oz::MSG_LOOKAT_WEIGHT);
        sendFloat(weight);
        lookat_weight = weight;
    }
}


float QClient1::lookAtWeight()
{
    if(is_connected)
        return lookat_weight;
    else
        return 0.0f;
}


void QClient1::initialize()
{
    short_name = tr("");
    long_name = tr("");
    is_speaking = false;
    is_gesturing = false;
    voice_gender_language = oz::VGL_DEFAULT;
    voice_volume = 1.0f;
    voice_pitch = 1.0f;
    gesture_weight = 1.0f;
    current_expression = oz::EXPRESSION_NEUTRAL;
    expression_weight = 1.0f;
    auto_blink_speed = 0.1f;
    random_head_weight = 0.0f;
    random_eyes_weight = 0.0f;
    random_face_weight = 0.0f;
    direct_weight = 0.0f;
    current_lookat = oz::LOOKAT_FORWARD;
    lookat_weight = 1.0f;
}


bool QClient1::sendExact(unsigned char* buffer,int size)
{
    int rem = size;
    unsigned char* bptr = buffer;
    while(rem > 0)
    {
        int chunk = rem;
        int bytes = socket.write((char*)bptr,chunk);
        if(bytes <= 0)
        {
            printf("cannot write, trying again\n");
            if(!socket.waitForBytesWritten(1000))
                return false;
            bytes = socket.write((char*)bptr,chunk);
        }
        rem -= bytes;
        bptr += bytes;
    }
    return true;
}


bool QClient1::receiveExact(unsigned char* buffer,int size)
{
    int rem = size;
    unsigned char* bptr = buffer;
    while(rem > 0)
    {
        int chunk = rem;
        int bytes = socket.read((char*)bptr,chunk);
        if(bytes <= 0)
        {
            printf("cannot read, trying again\n");
            if(!socket.waitForReadyRead(1000))
                return false;
            bytes = socket.read((char*)bptr,chunk);
            printf("now %d bytes\n",bytes);
        }
        rem -= bytes;
        bptr += bytes;
    }
    return true;
}


void QClient1::sendDword(unsigned int d)
{
    unsigned char b[4];
    b[0] = d >> 24;
    b[1] = (d >> 16) & 255;
    b[2] = (d >> 8) & 255;
    b[3] = d & 255;
    sendExact(b,4);
}


unsigned int QClient1::receiveDword()
{
    unsigned char b[4];
    receiveExact(b,4);
    return (b[0] << 24) | (b[1] << 16) | (b[2] << 8) | b[3];
}


void QClient1::sendFloat(float f)
{
    sendDword(*(unsigned int*)&f);
}


float QClient1::receiveFloat()
{
    unsigned int d = receiveDword();
    return *(float*)&d;
}


void QClient1::sendString(char* s)
{
    unsigned short w = strlen(s);
    unsigned char b[2];
    b[0] = w >> 8;
    b[1] = w & 255;
    sendExact(b,2);
    sendExact((unsigned char*)s,w);
}


void QClient1::receiveString(char* s,int max)
{
    unsigned short length;
    unsigned char b[2];
    receiveExact(b,2);
    length = (b[0] << 8) | b[1];
    if(length > max - 1)
    {
        receiveExact((unsigned char*)s,max - 1);
        s[max - 1] = 0;
        unsigned char dummy[65536];
        receiveExact(dummy,length - max + 1);
    }
    else
    {
        receiveExact((unsigned char*)s,length);
        s[length] = 0;
    }
}


void QClient1::socketReadyRead()
{
    if(is_connected)
    {
        int msg = receiveDword();
        switch(msg)
        {
            case oz::MSG_SPEAK_DONE:
                is_speaking = false;
                speakDone();
                speakDoneProxy();
                break;

            case oz::MSG_GESTURE_DONE:
                is_gesturing = false;
                gestureDone();
                gestureDoneProxy();
                break;

            case oz::MSG_SPEAK_GESTURE:
                {
                    int id = receiveDword();
                    gestureStart(id);
                    speakGesture(id);
                }
                break;

            case oz::MSG_UTTERANCE:
                {
                    Utterance utt;
                    utt.options = receiveDword();
                    for(int i = 0; i < utt.options; i++)
                    {
                        utt.option[i].confidence = receiveFloat();
                        receiveString(utt.option[i].text,MAX_CHARACTERS);
                    }
                    utterance(utt);
                }
                break;

            case oz::MSG_FACES:
                {
                    Faces fac;
                    fac.faces = receiveDword();
                    for(int i = 0; i < fac.faces; i++)
                    {
                        fac.face[i].confidence = receiveFloat();
                        fac.face[i].x = receiveFloat();
                        fac.face[i].y = receiveFloat();
                        fac.face[i].xsize = receiveFloat();
                        fac.face[i].ysize = receiveFloat();
                        receiveString(fac.face[i].identity,MAX_CHARACTERS);
                    }
                    faces(fac);
                }
                break;

            case oz::MSG_VIDEO_FRAME:
                {
                    int length = receiveDword();
                    receiveExact(video_frame,length);
                    jpegVideoFrame(video_frame,length);
                }
                break;

            default:
                printf("sync error\n");
                break;
        }
    }
}


void QClient1::socketConnected()
{
    sendDword(oz::MSG_REQUEST_VERSION);
    sendDword(0x01000000);
    if(receiveDword() != 0x01000000)
        socketDisconnected();
    char temp[1024];
    receiveString(temp,1024);
    short_name.fromUtf8(temp);
    receiveString(temp,1024);
    long_name.fromUtf8(temp);
    is_connected = true;
    reset();
    connected();
}


void QClient1::socketDisconnected()
{
    is_connected = false;
    disconnected();
    socketDisconnectedProxy();
    if(is_speaking)
    {
        is_speaking = false;
        speakDone();
        speakDoneProxy();
    }
    if(is_gesturing)
    {
        is_gesturing = false;
        gestureDone();
        gestureDoneProxy();
    }
}
