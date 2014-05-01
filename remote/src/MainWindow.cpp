#include "MainWindow.h"
#include "ConnectDialog.h"
#include <QMessageBox>
#include <QMenuBar>
#include <QFileDialog>
#include <QApplication>
#include <QStatusBar>
#include <QToolBar>
#include <QGraphicsView>
#include <QHostAddress>
#include <QHostInfo>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QThread>
#include <QTimer>


#define STREAM_XSIZE 640
#define STREAM_YSIZE 480
#define STREAM_FPS 20

#define OVERLAY_XSTART 20
#define OVERLAY_YSTART 360
#define OVERLAY_LINESPACE 10
#define MAX_OVERLAY_LINES 10


MainWindow::MainWindow(QApplication& aapplication,QWidget *parent) : QMainWindow(parent),application(aapplication),client(),client_ip(0),speak_lineedit(0),gesture_combobox(0),video(0),composited(0),speak_button(0),volume_slider(0),pitch_slider(0),gesture_button(0),expression_combobox(0),timer(0),recording_raw(false),recording_overlay(false)
{
    setAttribute(Qt::WA_DeleteOnClose);

    createActions();
    createMenus();
    createToolBar();

    // wire the client
    connect(&client,SIGNAL(connected()),this,SLOT(clientConnected()));
    connect(&client,SIGNAL(disconnected()),this,SLOT(clientDisconnected()));
    connect(&client,SIGNAL(speakStart()),this,SLOT(clientSpeakStart()));
    connect(&client,SIGNAL(speakDone()),this,SLOT(clientSpeakDone()));
    connect(&client,SIGNAL(gestureStart(int)),this,SLOT(clientGestureStart(int)));
    connect(&client,SIGNAL(gestureDone()),this,SLOT(clientGestureDone()));
    connect(&client,SIGNAL(speakGesture(int)),this,SLOT(clientSpeakGesture(int)));
    connect(&client,SIGNAL(utterance(Utterance&)),this,SLOT(clientUtterance(Utterance&)));
    connect(&client,SIGNAL(faces(Faces&)),this,SLOT(clientFaces(Faces&)));
    connect(&client,SIGNAL(jpegVideoFrame(unsigned char*,int)),this,SLOT(clientJpegVideoFrame(unsigned char*,int)));

    // example idioms

    // documentary
    /*idioms.append(tr("[braisel]Hallo, ik ben Alice"));
    idioms.append(tr("Hoe [braises]heet u"));
    idioms.append(tr("Wat is uw geboortedatum"));
    idioms.append(tr("Heeft u een [braises]partner"));
    idioms.append(tr("Heeft u [braises]kinderen"));
    idioms.append(tr("Woont u [braises]zelfstandig"));
    idioms.append(tr("Hoe vaak krijgt u [braises]hulp van de [braises]thuiszorg of [braises]andere zorginstellingen"));
    idioms.append(tr("Hoe lang woont u al in uw [braises]huidige woning"));
    idioms.append(tr("Gebruikt u [braises]medicatie"));
    idioms.append(tr("Vergeet u uw medicatie wel eens [braises]in te nemen"));
    idioms.append(tr("Wat voor [braises]werk deed u"));
    idioms.append(tr("Wat zijn uw [braises]hobbies"));
    idioms.append(tr("Voelt u zich wel eens [braises]eenzaam"));
    idioms.append(tr("Wanneer"));
    idioms.append(tr("Als u naar uw sociale [braises]leven kijkt, wat voor [braises]cijfer zou u dan geven"));
    idioms.append(tr("Hoe zou uw sociale leven kunnen [braises]verbeteren"));
    idioms.append(tr("Wie [braises]bezoekt u zoal"));
    idioms.append(tr("Waar wordt u [braises]blij van"));
    idioms.append(tr("Wat zou ik voor u kunnen [braises]betekenen"));
    idioms.append(tr("[yess]Ja, dat begrijp ik"));
    idioms.append(tr("[nos3]Nee, natuurlijk niet"));
    idioms.append(tr("[nos]Nee, dat lijkt me [braises]ook niet"));
    idioms.append(tr("[braises]Hallo, ik ben Alice, en [nos3]ik ben niet grappig."));*/

    idioms.append(tr("Hoi, ik ben [braises]Alice. Wat vind je ervan dat ik je, [braises]als robot, een paar vragen stel?"));
    idioms.append(tr("Ik ga je een paar vragen stellen over [braises]hoe je je leven ervaart. Ben je over het algemeen [braises]tevreden met je leven?"));
    idioms.append(tr("Goed om te weten. En hoe tevreden ben je over je [braises]lichamelijke gezondheid?"));
    idioms.append(tr("En hoe is het gesteld met je [braises]psychische gezondheid?"));
    idioms.append(tr("Heb je eigenlijk veel [braises]vrienden?"));
    idioms.append(tr("En heb je je vrienden ondanks nog gesproken?"));
    idioms.append(tr("Ben je tevreden over je [braises]vriendschappen?"));
    idioms.append(tr("Heb je een partner?"));
    idioms.append(tr("Ben je tevreden over je relatie met je [braises]partner?"));
    idioms.append(tr("En heb je contact met je familie?"));
    idioms.append(tr("Okee. En ben je tevreden over je [braises]familie en hoe jullie in de familie met elkaar [braises]omgaan?"));
    idioms.append(tr("Ben je tevreden met je [braises]woonsituatie?"));
    idioms.append(tr("Aha, dan zullen we daar rekening mee houden. En wat doe je zoal in het [braisel]dagelijks leven?"));
    idioms.append(tr("En hoe tevreden ben je daarmee?"));
    idioms.append(tr("Nu iets anders. Hoe zit het bijvoorbeeld met het geld, hoe staan de zaken er [braises]financieel voor?"));
    idioms.append(tr("Ben je daar tevreden over?"));
    idioms.append(tr("Nou, dank je wel voor het beantwoorden van mijn vragen!"));
    idioms.append(tr("Er volgen nu nog vragen over wat je van mij vindt na dit gesprek."));

    // central widget
    QWidget* remote = new QWidget(this);
    QGridLayout* layout = new QGridLayout();

    // speak
    QGroupBox* speak_box = new QGroupBox(tr("Speak"),remote);
    QVBoxLayout* speak_layout = new QVBoxLayout();

    QHBoxLayout* speakline_layout = new QHBoxLayout();
    speak_lineedit = new QLineEdit(speak_box);
    speakline_layout->addWidget(speak_lineedit);
    speak_button = new QPushButton(tr("Speak"),speak_box);
    connect(speak_button,SIGNAL(clicked()),this,SLOT(speakClicked()));
    speakline_layout->addWidget(speak_button);
    speak_layout->addLayout(speakline_layout);

    QHBoxLayout* volpitch_layout = new QHBoxLayout();
    volpitch_layout->addWidget(new QLabel(tr("Volume"),speak_box),1);
    volume_slider = new QSlider(Qt::Horizontal,speak_box);
    volume_slider->setRange(0,100);
    connect(volume_slider,SIGNAL(sliderMoved(int)),this,SLOT(volumeSliderMoved(int)));
    volpitch_layout->addWidget(volume_slider,3);
    volpitch_layout->addWidget(new QLabel(tr("Pitch"),speak_box),1);
    pitch_slider = new QSlider(Qt::Horizontal,speak_box);
    pitch_slider->setRange(50,200);
    connect(pitch_slider,SIGNAL(sliderMoved(int)),this,SLOT(pitchSliderMoved(int)));
    volpitch_layout->addWidget(pitch_slider,3);

    speak_layout->addLayout(volpitch_layout);
    speak_box->setLayout(speak_layout);

    layout->addWidget(speak_box,0,0,1,2);

    // gesture
    QGroupBox* gesture_box = new QGroupBox(tr("Gesture"),remote);
    QHBoxLayout* gesture_layout = new QHBoxLayout();
    gesture_combobox = new QComboBox(gesture_box);
    gesture_combobox->addItem(tr("brow raise long"));
    gesture_combobox->addItem(tr("brow raise short"));
    gesture_combobox->addItem(tr("frown long"));
    gesture_combobox->addItem(tr("frown short"));
    gesture_combobox->addItem(tr("yes long"));
    gesture_combobox->addItem(tr("yes short"));
    gesture_combobox->addItem(tr("yes long 3x"));
    gesture_combobox->addItem(tr("yes short 3x"));
    gesture_combobox->addItem(tr("no long"));
    gesture_combobox->addItem(tr("no short"));
    gesture_combobox->addItem(tr("no long 3x"));
    gesture_combobox->addItem(tr("no short 3x"));
    gesture_combobox->addItem(tr("indian wobble"));
    gesture_layout->addWidget(gesture_combobox,2);
    gesture_button = new QPushButton(tr("Perform"),gesture_box);
    connect(gesture_button,SIGNAL(clicked()),this,SLOT(gestureClicked()));
    gesture_layout->addWidget(gesture_button,1);
    gesture_box->setLayout(gesture_layout);

    layout->addWidget(gesture_box,1,0,1,1);

    // expression
    QGroupBox* expression_box = new QGroupBox(tr("Expression"),remote);
    QHBoxLayout* expression_layout = new QHBoxLayout();
    expression_combobox = new QComboBox(expression_box);
    expression_combobox->addItem(tr("neutral"));
    expression_combobox->addItem(tr("happy"));
    expression_combobox->addItem(tr("sad"));
    expression_combobox->addItem(tr("evil"));
    expression_combobox->addItem(tr("afraid"));
    connect(expression_combobox,SIGNAL(currentIndexChanged(int)),this,SLOT(expressionChanged(int)));
    expression_layout->addWidget(expression_combobox);
    expression_box->setLayout(expression_layout);

    layout->addWidget(expression_box,2,0,1,1);

    // eyes
    QGroupBox* eyes_box = new QGroupBox(tr("Eyes"),remote);
    QVBoxLayout* eyes_layout = new QVBoxLayout();

    QHBoxLayout* autoblink_layout = new QHBoxLayout();
    autoblink_layout->addWidget(new QLabel(tr("Blink Speed"),eyes_box),1);
    speed_slider = new QSlider(Qt::Horizontal,eyes_box);
    speed_slider->setRange(5,200);
    connect(speed_slider,SIGNAL(sliderMoved(int)),this,SLOT(blinkSpeedSliderMoved(int)));
    autoblink_layout->addWidget(speed_slider,3);
    eyes_layout->addLayout(autoblink_layout);

    QHBoxLayout* squint_layout = new QHBoxLayout();
    squint_layout->addWidget(new QLabel(tr("Squint"),eyes_box),1);
    QSlider* squint_slider = new QSlider(Qt::Horizontal,eyes_box);
    squint_slider->setRange(0,100);
    squint_slider->setEnabled(false);
    squint_layout->addWidget(squint_slider,3);
    eyes_layout->addLayout(squint_layout);

    lookat_combobox = new QComboBox(eyes_box);
    lookat_combobox->addItem(tr("forward"));
    lookat_combobox->addItem(tr("down"));
    lookat_combobox->addItem(tr("up"));
    lookat_combobox->addItem(tr("closest face"));
    lookat_combobox->addItem(tr("furthest face"));
    lookat_combobox->addItem(tr("away (avoid faces)"));
    connect(lookat_combobox,SIGNAL(currentIndexChanged(int)),this,SLOT(lookatChanged(int)));

    eyes_layout->addWidget(lookat_combobox);
    eyes_box->setLayout(eyes_layout);

    layout->addWidget(eyes_box,3,0,1,1);

    // mixer
    QGroupBox* mixer = new QGroupBox(tr("Mixer"),remote);
    QGridLayout* mixer_layout = new QGridLayout();
    gesture_slider = new QSlider(Qt::Vertical,mixer);
    gesture_slider->setRange(0,100);
    connect(gesture_slider,SIGNAL(sliderMoved(int)),this,SLOT(gestureSliderMoved(int)));
    mixer_layout->addWidget(gesture_slider,0,0);
    mixer_layout->addWidget(new QLabel(tr("gest.")),1,0);
    expression_slider = new QSlider(Qt::Vertical,mixer);
    expression_slider->setRange(0,100);
    connect(expression_slider,SIGNAL(sliderMoved(int)),this,SLOT(expressionSliderMoved(int)));
    mixer_layout->addWidget(expression_slider,0,1);
    mixer_layout->addWidget(new QLabel(tr("expr.")),1,1);
    lookat_slider = new QSlider(Qt::Vertical,mixer);
    lookat_slider->setRange(0,100);
    connect(lookat_slider,SIGNAL(sliderMoved(int)),this,SLOT(lookatSliderMoved(int)));
    mixer_layout->addWidget(lookat_slider,0,2);
    mixer_layout->addWidget(new QLabel(tr("look")),1,2);
    head_slider = new QSlider(Qt::Vertical,mixer);
    head_slider->setRange(0,100);
    connect(head_slider,SIGNAL(sliderMoved(int)),this,SLOT(headSliderMoved(int)));
    mixer_layout->addWidget(head_slider,0,3);
    mixer_layout->addWidget(new QLabel(tr("head")),1,3);
    eyes_slider = new QSlider(Qt::Vertical,mixer);
    eyes_slider->setRange(0,100);
    connect(eyes_slider,SIGNAL(sliderMoved(int)),this,SLOT(eyesSliderMoved(int)));
    mixer_layout->addWidget(eyes_slider,0,4);
    mixer_layout->addWidget(new QLabel(tr("eyes")),1,4);
    face_slider = new QSlider(Qt::Vertical,mixer);
    face_slider->setRange(0,100);
    connect(face_slider,SIGNAL(sliderMoved(int)),this,SLOT(faceSliderMoved(int)));
    mixer_layout->addWidget(face_slider,0,5);
    mixer_layout->addWidget(new QLabel(tr("face")),1,5);
    mixer->setLayout(mixer_layout);

    layout->addWidget(mixer,4,0,1,1);

    // soundboard
    QWidget* soundboard = new QGroupBox(tr("Idiom"),remote);
    QVBoxLayout* soundboard_layout = new QVBoxLayout();
    listwidget = new QListWidget(soundboard);
    listwidget->setMinimumWidth(200);
    for(QList<QString>::iterator i = idioms.begin(); i != idioms.end(); i++)
        listwidget->addItem(*i);
    connect(listwidget,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(idiomClicked(QListWidgetItem*)));
    soundboard_layout->addWidget(listwidget);
    soundboard->setLayout(soundboard_layout);
    layout->addWidget(soundboard,1,1,4,1);

    // streamer
    video = new QPixmap(STREAM_XSIZE,STREAM_YSIZE);
    video->fill(Qt::black);
    composited = new QPixmap(STREAM_XSIZE,STREAM_YSIZE);
    videolabel = new QLabel(remote);
    videolabel->setPixmap(*composited);
    layout->addWidget(videolabel,0,2,5,1);

    av_register_all();

    remote->setLayout(layout);
    setCentralWidget(remote);

    // timer
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(timerTick()));
    timer->start(1000 / STREAM_FPS);

    setWindowTitle("OzRemote");
    statusBar()->showMessage("Ready");

    disconnect_action->setEnabled(false);
    recno_action->setEnabled(false);
    recov_action->setEnabled(false);
    recb_action->setEnabled(false);
    stop_action->setEnabled(false);
    speak_lineedit->setEnabled(false);
    speak_button->setEnabled(false);
    volume_slider->setEnabled(false);
    pitch_slider->setEnabled(false);
    gesture_combobox->setEnabled(false);
    gesture_button->setEnabled(false);
    expression_combobox->setEnabled(false);
    speed_slider->setEnabled(false);
    lookat_combobox->setEnabled(false);
    gesture_slider->setEnabled(false);
    expression_slider->setEnabled(false);
    lookat_slider->setEnabled(false);
    head_slider->setEnabled(false);
    eyes_slider->setEnabled(false);
    face_slider->setEnabled(false);
    listwidget->setEnabled(false);
}


MainWindow::~MainWindow()
{
}


void MainWindow::createActions()
{
    connect_action = new QAction(QIcon(DATADIR "connect.png"),tr("&Connect"),this);
    connect(connect_action,SIGNAL(triggered()),this,SLOT(robotConnect()));
    disconnect_action = new QAction(QIcon(DATADIR "disconnect.png"),tr("&Disconnect"),this);
    connect(disconnect_action,SIGNAL(triggered()),this,SLOT(robotDisconnect()));
    exit_action = new QAction(tr("E&xit"),this);
    connect(exit_action,SIGNAL(triggered()),this,SLOT(robotExit()));

    options_action = new QAction(tr("&Options"),this);
    connect(options_action,SIGNAL(triggered()),this,SLOT(toolsOptions()));

    overview_action = new QAction(tr("&Overview"),this);
    connect(overview_action,SIGNAL(triggered()),this,SLOT(helpOverview()));
    about_action = new QAction(tr("&About"),this);
    connect(about_action,SIGNAL(triggered()),this,SLOT(helpAbout()));

    recno_action = new QAction(QIcon(DATADIR "recno.png"),tr("&Record"),this);
    connect(recno_action,SIGNAL(triggered()),this,SLOT(toolsRecord()));
    recov_action = new QAction(QIcon(DATADIR "recov.png"),tr("Record &Overlay"),this);
    connect(recov_action,SIGNAL(triggered()),this,SLOT(toolsRecordOverlay()));
    recb_action = new QAction(QIcon(DATADIR "recb.png"),tr("Record &Both"),this);
    connect(recb_action,SIGNAL(triggered()),this,SLOT(toolsRecordBoth()));
    stop_action = new QAction(QIcon(DATADIR "stop.png"),tr("Stop Recording"),this);
    connect(stop_action,SIGNAL(triggered()),this,SLOT(toolsStop()));
}


void MainWindow::createMenus()
{
    robot_menu = menuBar()->addMenu(tr("&Robot"));
    robot_menu->addAction(connect_action);
    robot_menu->addAction(disconnect_action);
    robot_menu->addSeparator();
    robot_menu->addAction(exit_action);

    tools_menu = menuBar()->addMenu(tr("&Tools"));
    tools_menu->addAction(options_action);
    tools_menu->addSeparator();
    tools_menu->addAction(recno_action);
    tools_menu->addAction(recov_action);
    tools_menu->addAction(recb_action);
    tools_menu->addAction(stop_action);

    help_menu = menuBar()->addMenu(tr("&Help"));
    help_menu->addAction(overview_action);
    help_menu->addSeparator();
    help_menu->addAction(about_action);
}


void MainWindow::createToolBar()
{
    QToolBar* toolbar = addToolBar(tr("Tools"));
    toolbar->addAction(connect_action);
    toolbar->addAction(disconnect_action);
    toolbar->addSeparator();
    toolbar->addAction(recno_action);
    toolbar->addAction(recov_action);
    toolbar->addAction(recb_action);
    toolbar->addAction(stop_action);
}


void MainWindow::robotConnect()
{
    ConnectDialog* dialog = new ConnectDialog(this);
    QDialog::DialogCode result = (QDialog::DialogCode)dialog->exec();
    QString name = dialog->name();
    QHostAddress address;
    quint16 port = dialog->port();
    delete dialog;

    if(result == QDialog::Accepted)
    {
        QStringList part = name.split(".");
        if(part.size() == 4)
            address = QHostAddress((part[0].toInt() << 24) | (part[1].toInt() << 16) | (part[2].toInt() << 8) | part[3].toInt());
        else if(name == "localhost")
            address = QHostAddress(QHostAddress::LocalHost);
        else
        {
            QList<QHostAddress> addresses = QHostInfo::fromName(name).addresses();
            if(addresses.size() == 0)
            {
                QMessageBox::warning(this,"Not Found","Host " + name + " not found");
                return;
            }
            address = addresses.first();
        }
        if(port == 0)
            port = oz::OZ_PORT;
        client.connectToHost(address,port);
        if(!client.waitForConnected())
            QMessageBox::warning(this,"Connection Error","Unable to connect to robot at " + name + ":" + QString::number(port));
    }
}


void MainWindow::robotDisconnect()
{
    client.disconnectFromHost();
}


void MainWindow::robotExit()
{
    application.closeAllWindows();
}


void MainWindow::toolsOptions()
{
    // TODO: show options dialog for this model
}


void MainWindow::toolsRecord()
{
    QString basename = QFileDialog::getSaveFileName(this,tr("Record Video"),"",tr("Windows Media Format (*.wmv);;Windows AVI (*.avi);;MPEG-4 (*.mp4);;Matroska (*.wmv);;Ogg/Vorbis (*.ogg);;Any File (*.*)"));
    if(!basename.isNull())
    {
        QStringList part = basename.split(".");
        QString filename = part[0];
        QString extension = ".wmv";
        if(part.size() >= 2)
            extension = part[part.size() - 1];
        startRecording(filename,extension,true,false);
        recno_action->setEnabled(false);
        recov_action->setEnabled(false);
        recb_action->setEnabled(false);
        stop_action->setEnabled(true);
    }
}


void MainWindow::toolsRecordOverlay()
{
    QString basename = QFileDialog::getSaveFileName(this,tr("Record Video with Overlay"),"",tr("Windows Media Format (*.wmv);;Windows AVI (*.avi);;MPEG-4 (*.mp4);;Matroska (*.wmv);;Ogg/Vorbis (*.ogg);;Any File (*.*)"));
    if(!basename.isNull())
    {
        QStringList part = basename.split(".");
        QString filename = part[0];
        QString extension = ".wmv";
        if(part.size() >= 2)
            extension = part[part.size() - 1];
        startRecording(filename,extension,false,true);
        recno_action->setEnabled(false);
        recov_action->setEnabled(false);
        recb_action->setEnabled(false);
        stop_action->setEnabled(true);
    }
}


void MainWindow::toolsRecordBoth()
{
    QString basename = QFileDialog::getSaveFileName(this,tr("Record Video and Video with Overlay"),"",tr("Windows Media Format (*.wmv);;Windows AVI (*.avi);;MPEG-4 (*.mp4);;Matroska (*.wmv);;Ogg/Vorbis (*.ogg);;Any File (*.*)"));
    if(!basename.isNull())
    {
        QStringList part = basename.split(".");
        QString filename = part[0];
        QString extension = ".wmv";
        if(part.size() >= 2)
            extension = part[part.size() - 1];
        startRecording(filename,extension,true,true);
        recno_action->setEnabled(false);
        recov_action->setEnabled(false);
        recb_action->setEnabled(false);
        stop_action->setEnabled(true);
    }
}


void MainWindow::toolsStop()
{
    stopRecording();
    recno_action->setEnabled(true);
    recov_action->setEnabled(true);
    recb_action->setEnabled(true);
    stop_action->setEnabled(false);
}


void MainWindow::timerTick()
{
    if(recording_raw)
    {
        QImage image(video->toImage().convertToFormat(QImage::Format_ARGB32));
        char* out_buffer = (char*)av_malloc(image.height() * image.width() * 3 / 2);
        AVFrame* inpic = av_frame_alloc();
        AVFrame* outpic = av_frame_alloc();
        avpicture_fill((AVPicture*)inpic,image.bits(),AV_PIX_FMT_BGRA,image.width(),image.height());
        avpicture_fill((AVPicture*)outpic,(const uint8_t*)out_buffer,AV_PIX_FMT_YUV420P,image.width(),image.height());
        SwsContext* ctx = sws_getContext(image.width(),image.height(),AV_PIX_FMT_BGRA,image.width(),image.height(),AV_PIX_FMT_YUV420P,SWS_BICUBIC,NULL,NULL,NULL);
        sws_scale(ctx,inpic->data,inpic->linesize,0,image.height(),outpic->data,outpic->linesize);
        av_free(inpic);
        AVPacket packet = { 0 };
        av_init_packet(&packet);
        outpic->pts = vcount_raw++;
        int got_packet = 0;
        if(avcodec_encode_video2(vstream_raw->codec,&packet,outpic,&got_packet) >= 0)
            if(got_packet)
                writePacket(*avcontext_raw,vstream_raw->codec->time_base,*vstream_raw,packet);
        av_free(outpic);
        av_free(out_buffer);
    }

    if(recording_overlay)
    {
        QImage image(composited->toImage().convertToFormat(QImage::Format_ARGB32));
        char* out_buffer = (char*)av_malloc(image.height() * image.width() * 3 / 2);
        AVFrame* inpic = av_frame_alloc();
        AVFrame* outpic = av_frame_alloc();
        avpicture_fill((AVPicture*)inpic,image.bits(),AV_PIX_FMT_BGRA,image.width(),image.height());
        avpicture_fill((AVPicture*)outpic,(const uint8_t*)out_buffer,AV_PIX_FMT_YUV420P,image.width(),image.height());
        SwsContext* ctx = sws_getContext(image.width(),image.height(),AV_PIX_FMT_BGRA,image.width(),image.height(),AV_PIX_FMT_YUV420P,SWS_BICUBIC,NULL,NULL,NULL);
        sws_scale(ctx,inpic->data,inpic->linesize,0,image.height(),outpic->data,outpic->linesize);
        av_free(inpic);
        AVPacket packet = { 0 };
        av_init_packet(&packet);
        outpic->pts = vcount_overlay++;
        int got_packet = 0;
        if(avcodec_encode_video2(vstream_overlay->codec,&packet,outpic,&got_packet) >= 0)
            if(got_packet)
                writePacket(*avcontext_overlay,vstream_overlay->codec->time_base,*vstream_overlay,packet);
        av_free(outpic);
        av_free(out_buffer);
    }
}


void MainWindow::helpOverview()
{
    // TODO: show modal dialog with html viewer with help stuff
}


void MainWindow::helpAbout()
{
    QMessageBox::about(this,tr("About Oz Remote 0.1"),tr("Oz Robot Puppeteering System is a small and simple humanoid social robot experimenting suite.\nOzRemote lets you control a robot remotely.\nThe current version is 0.1\n(C) Copyrights Germans Media Technology & Services\n"));
}


void MainWindow::speakClicked()
{
    QString text = tr("Speak: ") + speak_lineedit->text();
    writeOverlay(text);
    client.speak(speak_lineedit->text());
}


void MainWindow::volumeSliderMoved(int value)
{
    client.setVoiceVolume((float)value / 100.0f);
}


void MainWindow::pitchSliderMoved(int value)
{
    client.setVoicePitch((float)value / 100.0f);
}


void MainWindow::gestureClicked()
{
    client.gesture(gesture_combobox->currentIndex());
}


void MainWindow::expressionChanged(int index)
{
    client.setExpression(index);
}


void MainWindow::blinkSpeedSliderMoved(int value)
{
    client.setAutoBlinkSpeed((float)value / 100.0f);
}


void MainWindow::lookatChanged(int index)
{
    client.setLookAt(index);
}


void MainWindow::gestureSliderMoved(int value)
{
    client.setGestureWeight((float)value / 100.0f);
}


void MainWindow::expressionSliderMoved(int value)
{
    client.setExpressionWeight((float)value / 100.0f);
}


void MainWindow::lookatSliderMoved(int value)
{
    client.setLookAtWeight((float)value / 100.0f);
}


void MainWindow::headSliderMoved(int value)
{
    client.setRandomHeadWeight((float)value / 100.0f);
}


void MainWindow::eyesSliderMoved(int value)
{
    client.setRandomEyesWeight((float)value / 100.0f);
}


void MainWindow::faceSliderMoved(int value)
{
    client.setRandomFaceWeight((float)value / 100.0f);
}


void MainWindow::idiomClicked(QListWidgetItem* item)
{
    QString text = tr("Speak: ") + item->text();
    writeOverlay(text);
    client.speak(item->text());
}


void MainWindow::clientConnected()
{
    connect_action->setEnabled(false);
    disconnect_action->setEnabled(true);
    recno_action->setEnabled(true);
    recov_action->setEnabled(true);
    recb_action->setEnabled(true);
    stop_action->setEnabled(false);
    speak_lineedit->setEnabled(true);
    speak_button->setEnabled(true);
    volume_slider->setEnabled(true);
    volume_slider->setValue((int)(client.voiceVolume() * 100.0f));
    pitch_slider->setEnabled(true);
    volume_slider->setValue((int)(client.voicePitch() * 100.0f));
    gesture_combobox->setEnabled(true);
    gesture_combobox->setCurrentIndex(0);
    gesture_button->setEnabled(true);
    expression_combobox->setEnabled(true);
    expression_combobox->setCurrentIndex(client.expression());
    speed_slider->setEnabled(true);
    speed_slider->setValue((int)(client.autoBlinkSpeed() * 100.0f));
    lookat_combobox->setEnabled(true);
    lookat_combobox->setCurrentIndex(client.lookAt());
    gesture_slider->setEnabled(true);
    gesture_slider->setValue((int)(client.gestureWeight() * 100.0f));
    expression_slider->setEnabled(true);
    expression_slider->setValue((int)(client.expressionWeight() * 100.0f));
    lookat_slider->setEnabled(true);
    lookat_slider->setValue((int)(client.lookAtWeight() * 100.0f));
    head_slider->setEnabled(true);
    head_slider->setValue((int)(client.randomHeadWeight() * 100.0f));
    eyes_slider->setEnabled(true);
    eyes_slider->setValue((int)(client.randomEyesWeight() * 100.0f));
    face_slider->setEnabled(true);
    face_slider->setValue((int)(client.randomFaceWeight() * 100.0f));
    listwidget->setEnabled(true);
    setWindowTitle("OzRemote (" + client.longName() + ")");
}


void MainWindow::clientDisconnected()
{
    if(recording_raw || recording_overlay)
        toolsStop();
    disconnect_action->setEnabled(false);
    connect_action->setEnabled(true);
    recno_action->setEnabled(false);
    recov_action->setEnabled(false);
    recb_action->setEnabled(false);
    stop_action->setEnabled(false);
    speak_lineedit->setEnabled(false);
    speak_lineedit->setText("");
    speak_button->setEnabled(false);
    volume_slider->setEnabled(false);
    volume_slider->setValue(0);
    pitch_slider->setEnabled(false);
    pitch_slider->setEnabled(0);
    gesture_combobox->setEnabled(false);
    gesture_combobox->setCurrentIndex(0);
    gesture_button->setEnabled(false);
    expression_combobox->setEnabled(false);
    expression_combobox->setCurrentIndex(0);
    speed_slider->setEnabled(false);
    speed_slider->setValue(0);
    lookat_combobox->setEnabled(false);
    lookat_combobox->setCurrentIndex(0);
    gesture_slider->setEnabled(false);
    gesture_slider->setValue(0);
    expression_slider->setEnabled(false);
    expression_slider->setValue(0);
    lookat_slider->setEnabled(false);
    lookat_slider->setValue(0);
    head_slider->setEnabled(false);
    head_slider->setValue(0);
    eyes_slider->setEnabled(false);
    eyes_slider->setValue(0);
    face_slider->setEnabled(false);
    face_slider->setValue(0);
    listwidget->setEnabled(false);
    listwidget->clearSelection();
    video->fill(Qt::black);
    current_faces.faces = 0;
    overlay_text.clear();
    compose();
    setWindowTitle("OzRemote");
}


void MainWindow::clientSpeakStart()
{
    statusBar()->showMessage("Speaking...");
}


void MainWindow::clientSpeakDone()
{
    statusBar()->showMessage("Ready");
}


void MainWindow::clientGestureStart(int id)
{
    // TODO: notify which gesture started
}


void MainWindow::clientGestureDone()
{
    // TODO: notify that gesture is done
}


void MainWindow::clientSpeakGesture(int id)
{
    // TODO: notify that robot does gesture id as per parsed speech
}


void MainWindow::clientUtterance(Utterance& utterance)
{
    QString text = tr("Heard: ") + QString(utterance.option[0].text);
    writeOverlay(text);
}


void MainWindow::clientFaces(Faces& faces)
{
    current_faces = faces;
    compose();
}


void MainWindow::clientJpegVideoFrame(unsigned char* jpeg,int length)
{
    if(!video->loadFromData(jpeg,length))
        printf("unable to decode jpeg\n");
    else
        compose();
}


void MainWindow::compose()
{
    QPainter painter(composited);
    painter.drawPixmap(0,0,*video);
    painter.setPen(Qt::red);
    for(int i = 0; i < current_faces.faces; i++)
    {
        float x = current_faces.face[i].x;
        float y = current_faces.face[i].y;
        float xs = current_faces.face[i].xsize;
        float ys = current_faces.face[i].ysize;
        printf("face %f,%f (%fx%f)\n",x,y,xs,ys);
        QPointF pos(0.5f * (1.0f + x) * (float)STREAM_XSIZE,0.5f * (1.0f + y) * (float)STREAM_YSIZE);
        painter.drawEllipse(pos,0.5f * xs * (float)STREAM_XSIZE,0.5f * ys * (float)STREAM_YSIZE);
        //painter.drawText(pos,current_faces.face[i].identity);
        painter.drawText(pos,"John Connor");
    }

    painter.setPen(Qt::green);
    QPoint pos(OVERLAY_XSTART,OVERLAY_YSTART);
    for(int i = 0; i < overlay_text.size(); i++)
    {
        painter.drawText(pos,overlay_text.at(i));
        pos.setY(pos.y() + OVERLAY_LINESPACE);
    }

    videolabel->setPixmap(*composited);
    videolabel->repaint();
}


void MainWindow::startRecording(QString& basename,QString& extension,bool raw,bool overlay)
{
    QString rawname,ovlname;
    if(raw)
        if(overlay)
        {
            rawname = basename + "." + extension;
            ovlname = basename + "-overlay." + extension;
        }
        else
            rawname = basename + "." + extension;
    else
        ovlname = basename + "." + extension;

    // open avcontext
    if(raw)
    {
        avformat_alloc_output_context2(&avcontext_raw,0,0,rawname.toUtf8().data());
        if(!avcontext_raw)
        {
            QMessageBox::warning(this,"AV Context","Cannot create AV context");
            return;
        }

        vcodec_raw = avcodec_find_encoder(avcontext_raw->oformat->video_codec);
        if(!vcodec_raw)
        {
            QMessageBox::warning(this,"AV Codec","Cannot find video encoder");
            return;
        }

        vstream_raw = avformat_new_stream(avcontext_raw,vcodec_raw);
        if(!vstream_raw)
        {
            QMessageBox::warning(this,"AV Codec","Cannot allocate stream\n");
            return;
        }
        vstream_raw->id = avcontext_raw->nb_streams - 1;
        vstream_raw->codec->codec_id = avcontext_raw->oformat->video_codec;
        vstream_raw->codec->bit_rate = 1000000;
        vstream_raw->codec->width = STREAM_XSIZE;
        vstream_raw->codec->height = STREAM_YSIZE;
        vstream_raw->codec->time_base.num = 1;
        vstream_raw->codec->time_base.den = STREAM_FPS;
        vstream_raw->codec->gop_size = 12;
        vstream_raw->codec->pix_fmt = AV_PIX_FMT_YUV420P;
        if(vstream_raw->codec->codec_id == AV_CODEC_ID_MPEG1VIDEO)
            vstream_raw->codec->mb_decision = 2;
        if(avcontext_raw->oformat->flags & AVFMT_GLOBALHEADER)
            vstream_raw->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;

        if(avcodec_open2(vstream_raw->codec,vcodec_raw,0) < 0)
        {
            QMessageBox::warning(this,"AV Codec","Cannot open video codec\n");
            return;
        }

        if(avio_open(&avcontext_raw->pb,rawname.toUtf8().data(),AVIO_FLAG_WRITE))
        {
            QMessageBox::warning(this,"File","Cannot open file");
            return;
        }
        if(avformat_write_header(avcontext_raw,0) < 0)
        {
            QMessageBox::warning(this,"File","Cannot write header\n");
            return;
        }

        vcount_raw = 0;

        recording_raw = true;
    }
    if(overlay)
    {
        avformat_alloc_output_context2(&avcontext_overlay,0,0,ovlname.toUtf8().data());
        if(!avcontext_raw)
        {
            QMessageBox::warning(this,"AV Context","Cannot create AV context");
            return;
        }

        vcodec_overlay = avcodec_find_encoder(avcontext_overlay->oformat->video_codec);
        if(!vcodec_overlay)
        {
            QMessageBox::warning(this,"AV Codec","Cannot find video encoder");
            return;
        }

        vstream_overlay = avformat_new_stream(avcontext_overlay,vcodec_overlay);
        if(!vstream_overlay)
        {
            QMessageBox::warning(this,"AV Codec","Cannot allocate stream\n");
            return;
        }
        vstream_overlay->id = avcontext_overlay->nb_streams - 1;
        vstream_overlay->codec->codec_id = avcontext_overlay->oformat->video_codec;
        vstream_overlay->codec->bit_rate = 1000000;
        vstream_overlay->codec->width = STREAM_XSIZE;
        vstream_overlay->codec->height = STREAM_YSIZE;
        vstream_overlay->codec->time_base.num = 1;
        vstream_overlay->codec->time_base.den = STREAM_FPS;
        vstream_overlay->codec->gop_size = 12;
        vstream_overlay->codec->pix_fmt = AV_PIX_FMT_YUV420P;
        if(vstream_overlay->codec->codec_id == AV_CODEC_ID_MPEG1VIDEO)
            vstream_overlay->codec->mb_decision = 2;
        if(avcontext_overlay->oformat->flags & AVFMT_GLOBALHEADER)
            vstream_overlay->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;

        if(avcodec_open2(vstream_overlay->codec,vcodec_overlay,0) < 0)
        {
            QMessageBox::warning(this,"AV Codec","Cannot open video codec\n");
            return;
        }

        if(avio_open(&avcontext_overlay->pb,ovlname.toUtf8().data(),AVIO_FLAG_WRITE))
        {
            QMessageBox::warning(this,"File","Cannot open file");
            return;
        }
        if(avformat_write_header(avcontext_overlay,0) < 0)
        {
            QMessageBox::warning(this,"File","Cannot write header\n");
            return;
        }

        vcount_overlay = 0;

        recording_overlay = true;
    }
}


void MainWindow::stopRecording()
{
    if(recording_raw)
    {
        av_write_trailer(avcontext_raw);
        avio_close(avcontext_raw->pb);
        avcodec_close(vstream_raw->codec);
        avformat_free_context(avcontext_raw);
        recording_raw = false;
    }
    if(recording_overlay)
    {
        av_write_trailer(avcontext_overlay);
        avio_close(avcontext_overlay->pb);
        avcodec_close(vstream_overlay->codec);
        avformat_free_context(avcontext_overlay);
        recording_overlay = false;
    }
}


void MainWindow::writePacket(AVFormatContext& avcontext,const AVRational& time_base,AVStream& stream,AVPacket& packet)
{
    packet.pts = av_rescale_q_rnd(packet.pts,time_base,stream.time_base,(AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
    packet.dts = av_rescale_q_rnd(packet.dts,time_base,stream.time_base,(AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
    packet.duration = av_rescale_q(packet.duration,time_base,stream.time_base);
    packet.stream_index = stream.index;
    av_interleaved_write_frame(&avcontext,&packet);
}


void MainWindow::writeOverlay(QString& text)
{
    overlay_text.append(text);
    if(overlay_text.size() > MAX_OVERLAY_LINES)
        overlay_text.removeAt(0);
    compose();
}
