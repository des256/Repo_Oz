#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "QClient1.h"
#include <QMainWindow>
#include <QAction>
#include <QMenu>
#include <QGraphicsView>
#include <QLineEdit>
#include <QComboBox>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QTcpSocket>
#include <QPushButton>
#include <QListWidget>
#include <QLabel>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}


#define DATADIR "../../data/"


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QApplication& aapplication,QWidget* parent = 0);
    ~MainWindow();

public slots:
    void robotConnect();
    void robotDisconnect();
    void robotExit();

    void toolsOptions();
    void toolsRecord();
    void toolsRecordOverlay();
    void toolsRecordBoth();
    void toolsStop();

    void timerTick();

    void helpOverview();
    void helpAbout();

    void speakClicked();
    void volumeSliderMoved(int value);
    void pitchSliderMoved(int value);
    void gestureClicked();
    void expressionChanged(int index);
    void blinkSpeedSliderMoved(int value);
    void lookatChanged(int index);
    void gestureSliderMoved(int value);
    void expressionSliderMoved(int value);
    void lookatSliderMoved(int value);
    void headSliderMoved(int value);
    void eyesSliderMoved(int value);
    void faceSliderMoved(int value);

    void idiomClicked(QListWidgetItem* item);

    void clientConnected();
    void clientDisconnected();
    void clientSpeakStart();
    void clientSpeakDone();
    void clientGestureStart(int id);
    void clientGestureDone();
    void clientSpeakGesture(int id);
    void clientUtterance(Utterance& utterance);
    void clientFaces(Faces& faces);
    void clientJpegVideoFrame(unsigned char* jpeg,int length);

private:
    void createActions();
    void createMenus();
    void createToolBar();

    void compose();

    void startRecording(QString& basename,QString& extension,bool raw,bool overlay);
    void stopRecording();

    void writePacket(AVFormatContext& avcontext,const AVRational& time_base,AVStream& stream,AVPacket& packet);

    void writeOverlay(QString& text);

    QApplication& application;

    QClient1 client;
    Faces current_faces;

    bool recording_raw,recording_overlay;

    unsigned int client_ip;
    QLineEdit* speak_lineedit;
    QPushButton* speak_button;
    QSlider* volume_slider;
    QSlider* pitch_slider;
    QComboBox* gesture_combobox;
    QPushButton* gesture_button;
    QComboBox* expression_combobox;
    QSlider* speed_slider;
    QComboBox* lookat_combobox;
    QSlider* gesture_slider;
    QSlider* expression_slider;
    QSlider* lookat_slider;
    QSlider* head_slider;
    QSlider* eyes_slider;
    QSlider* face_slider;
    QListWidget* listwidget;

    QLabel* videolabel;
    QPixmap* video;
    QPixmap* composited;

    QTimer* timer;

    QList<QString> idioms;

    QAction* connect_action;
    QAction* disconnect_action;
    QAction* exit_action;

    QAction* options_action;
    QAction* recno_action;
    QAction* recov_action;
    QAction* recb_action;
    QAction* stop_action;

    QAction* overview_action;
    QAction* about_action;

    QMenu* robot_menu;
    QMenu* tools_menu;
    QMenu* help_menu;

    int vcount_raw,vcount_overlay;
    AVFormatContext* avcontext_raw,* avcontext_overlay;
    AVStream* vstream_raw,* vstream_overlay;
    AVCodec* vcodec_raw,* vcodec_overlay;

    QStringList overlay_text;
};


#endif // MAINWINDOW_H
