#ifndef QCLIENT1_H
#define QCLIENT1_H

#include <QObject>
#include <QHostAddress>
#include <QTcpSocket>
#include "../protocol.h"


#define MAX_CHARACTERS 1024
#define MAX_OPTIONS 16
#define MAX_FACES 16


struct Utterance
{
    int options;
    struct
    {
        float confidence;
        char text[MAX_CHARACTERS];
    } option[MAX_OPTIONS];

    Utterance() : options(0) { };
    Utterance(const Utterance& a) : options(a.options) { for(int i = 0; i < options; i++) { option[i].confidence = a.option[i].confidence; memcpy(option[i].text,a.option[i].text,MAX_CHARACTERS); } };
    Utterance& operator=(const Utterance& a) { if(&a != this) { options = a.options; for(int i = 0; i < options; i++) { option[i].confidence = a.option[i].confidence; memcpy(option[i].text,a.option[i].text,MAX_CHARACTERS); } } return *this; };
};


struct Faces
{
    int faces;
    struct
    {
        float confidence;
        float x,y;
        float xsize,ysize;
        char identity[MAX_CHARACTERS];
    } face[MAX_FACES];

    Faces() : faces(0) { };
    Faces(const Faces& a) : faces(a.faces) { for(int i = 0; i < faces; i++) { face[i].confidence = a.face[i].confidence; face[i].x = a.face[i].x; face[i].y = a.face[i].y; face[i].xsize = face[i].xsize; face[i].ysize = face[i].ysize; memcpy(face[i].identity,a.face[i].identity,MAX_CHARACTERS); } };
    Faces& operator=(const Faces& a) { if(&a != this) { faces = a.faces; for(int i = 0; i < faces; i++) { face[i].confidence = a.face[i].confidence; face[i].x = a.face[i].x; face[i].y = a.face[i].y; face[i].xsize = a.face[i].xsize; face[i].ysize = a.face[i].ysize; memcpy(face[i].identity,a.face[i].identity,MAX_CHARACTERS); } } return *this; };
};


class QClient1 : public QObject
{
    Q_OBJECT

public:
    QClient1();
    ~QClient1();

    // network connection
    void connectToHost(const QString& hostName,quint16 port,QIODevice::OpenMode openMode = QIODevice::ReadWrite,QAbstractSocket::NetworkLayerProtocol protocol = QAbstractSocket::AnyIPProtocol);
    void connectToHost(const QHostAddress& address,quint16 port,QIODevice::OpenMode openMode = QIODevice::ReadWrite);
    bool waitForConnected(int msecs = 30000);
    void disconnectFromHost();

    // administrative
    void reset();        // reset the robot
    QString name();      // returns name of the robot (i.e. "Alice")
    QString longName();  // returns long name of the robot (i.e. "Robokind R-50 Alice (Mint Linux blabla)")

    // speaking
    void speak(QString& text);             // speak text
    void waitForSpeak();                   // waits for speak done event from the robot
    bool speaking();                       // returns true if robot is speaking, false if not
    void setVoiceGenderLanguage(int vgl);  // set voice, gender and language (one of VGL_*)
    int voiceGenderLanguage();             // returns currently set voice, gender and language
    void setVoiceVolume(float volume);     // set voice volume (0..1)
    float voiceVolume();                   // returns currently set voice volume
    void setVoicePitch(float pitch);       // set voice pitch (0.5..2)
    float voicePitch();                    // returns curently set voice pitch

    // gestures
    void gesture(int id);                 // start gesture manually, one of GESTURE_*
    void waitForGesture();                // waits for gesture done event from the robot
    bool gesturing();                     // returns true if robot is gesturing, false if not
    void setGestureWeight(float weight);  // set gesture weight (0..1)
    float gestureWeight();                // returns currently set gesture weight

    // facial expression
    void setExpression(int expr);            // set facial expression (one of EXPR_*)
    int expression();                        // returns currently set facial expression
    void setExpressionWeight(float weight);  // set facial expression weight (0..1)
    float expressionWeight();                // returns currently set expression weight

    // automatic motion
    void setAutoBlinkSpeed(float speed);     // set automatic blink speed in Hz (0..10)
    float autoBlinkSpeed();                  // returns currently set automatic blink speed
    void setRandomHeadWeight(float weight);  // set random head motion weight (0..1)
    float randomHeadWeight();                // returns currently set random head motion weight
    void setRandomEyesWeight(float weight);  // set random eyes motion weight (0..1)
    float randomEyesWeight();                // returns currently set random eyes motion weight
    void setRandomFaceWeight(float weight);  // set random face motion weight (0..1)
    float randomFaceWeight();                // returns currently set random face motion weight

    // direct servo control
    void setServo(int id,float pos);     // set servo position (OZ_SERVO_*, 0..1)
    float servo(int id);                 // returns currently set servo position
    void setAllServos(float* pos);       // set all servos (0..1)
    void allServos(float* pos);          // returns all currently set servo positions
    void setDirectWeight(float weight);  // set direct servo control weight (0..1)
    float directWeight();                // returns currently set direct servo control weight

    // looking at faces
    void setLookAt(int target);          // set lookat target, one of LOOKAT_*
    int lookAt();                        // returns currently set lookat target
    void setLookAtWeight(float weight);  // set lookat weight (0..1)
    float lookAtWeight();                // returns currently set lookat weight

public slots:
    void socketReadyRead();
    void socketConnected();
    void socketDisconnected();

signals:
    void connected();
    void disconnected();
    void speakStart();
    void speakDone();
    void gestureStart(int id);
    void gestureDone();
    void speakGesture(int id);
    void utterance(Utterance& a);
    void faces(Faces& a);
    void jpegVideoFrame(unsigned char* jpeg,int length);

    void socketDisconnectedProxy();
    void speakDoneProxy();
    void gestureDoneProxy();

private:
    void initialize();
    bool sendExact(unsigned char* buffer,int size);
    bool receiveExact(unsigned char* buffer,int size);
    void sendDword(unsigned int d);
    void sendFloat(float f);
    unsigned int receiveDword();
    float receiveFloat();
    void sendString(char* s);
    void receiveString(char* s,int max);

    QTcpSocket socket;
    bool is_connected;
    QString short_name;
    QString long_name;
    bool is_speaking;
    bool is_gesturing;
    int voice_gender_language;
    float voice_volume;
    float voice_pitch;
    float gesture_weight;
    int current_expression;
    float expression_weight;
    float auto_blink_speed;
    float random_head_weight;
    float random_eyes_weight;
    float random_face_weight;
    float servo_pos[oz::MAX_SERVOS];
    float direct_weight;
    int current_lookat;
    float lookat_weight;
    unsigned char* video_frame;
};


#endif // QCLIENT1_H
