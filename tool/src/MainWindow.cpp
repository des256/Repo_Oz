#include "mainwindow.h"
#include "ConnectDialog.h"
#include "Command.h"
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


MainWindow::MainWindow(QApplication& aapplication,QWidget *parent) : QMainWindow(parent),application(aapplication),model(0),filename(),canvas(0),client_ip(0),speak_lineedit(0),gesture_combobox(0),mediaplayer(0),video(0),streamer(0),speak_button(0),volume_slider(0),pitch_slider(0),gesture_button(0),expression_combobox(0)
{
    setAttribute(Qt::WA_DeleteOnClose);
    createActions();
    createMenus();
    createToolBar();

    model = new Model(*this);

    mediaplayer = new QMediaPlayer(this,QMediaPlayer::StreamPlayback);

    // example idioms
    idioms.insert(tr("Hallo allemaal!"));
    idioms.insert(tr("Ik ben Alice, de zorgrobot."));
    idioms.insert(tr("We gaan nu eerst wat vragen beantwoorden."));
    idioms.insert(tr("Voelt U zich nu blij?"));
    idioms.insert(tr("En nu?"));
    idioms.insert(tr("Heeft U vrienden?"));
    idioms.insert(tr("Waarom niet?"));
    idioms.insert(tr("Ja, dat begrijp ik."));

    // tab widget
    QTabWidget* tabs = new QTabWidget(this);

    // remote control tab
    QWidget* remote = new QWidget(tabs);
    QHBoxLayout* remote_layout = new QHBoxLayout();

    // controls
    QWidget* left = new QWidget(remote);
    QVBoxLayout* left_layout = new QVBoxLayout();

    // speak
    QGroupBox* speak_box = new QGroupBox(tr("Speak"),left);
    QGridLayout* speak_layout = new QGridLayout();
    speak_lineedit = new QLineEdit(speak_box);
    speak_layout->addWidget(speak_lineedit,0,0,1,2);
    speak_button = new QPushButton(tr("Speak"),speak_box);
    connect(speak_button,SIGNAL(clicked()),this,SLOT(speakClicked()));
    speak_layout->addWidget(speak_button,0,2);
    speak_layout->addWidget(new QLabel(tr("Volume"),speak_box),1,0);
    volume_slider = new QSlider(Qt::Horizontal,speak_box);
    volume_slider->setRange(0,100);
    connect(volume_slider,SIGNAL(sliderMoved(int)),this,SLOT(volumeSliderMoved(int)));
    speak_layout->addWidget(volume_slider,1,1,1,2);
    speak_layout->addWidget(new QLabel(tr("Pitch"),speak_box),2,0);
    pitch_slider = new QSlider(Qt::Horizontal,speak_box);
    pitch_slider->setRange(50,200);
    connect(pitch_slider,SIGNAL(sliderMoved(int)),this,SLOT(pitchSliderMoved(int)));
    speak_layout->addWidget(pitch_slider,2,1,1,2);
    speak_box->setLayout(speak_layout);
    left_layout->addWidget(speak_box);

    // gesture
    QGroupBox* gesture_box = new QGroupBox(tr("Gesture"),left);
    QHBoxLayout* gesture_layout = new QHBoxLayout();
    gesture_combobox = new QComboBox(gesture_box);
    gesture_combobox->addItem(tr("slow yes (syes)"));
    gesture_combobox->addItem(tr("quick yes (qyes)"));
    gesture_combobox->addItem(tr("slow no (sno)"));
    gesture_combobox->addItem(tr("quick no (qno)"));
    gesture_layout->addWidget(gesture_combobox,2);
    gesture_button = new QPushButton(tr("Perform"),gesture_box);
    connect(gesture_button,SIGNAL(clicked()),this,SLOT(gestureClicked()));
    gesture_layout->addWidget(gesture_button,1);
    gesture_box->setLayout(gesture_layout);
    left_layout->addWidget(gesture_box);

    // expression
    QGroupBox* expression_box = new QGroupBox(tr("Expression"),left);
    QHBoxLayout* expression_layout = new QHBoxLayout();
    expression_combobox = new QComboBox(expression_box);
    expression_combobox->addItem(tr("happy"));
    expression_combobox->addItem(tr("sad"));
    expression_combobox->addItem(tr("evil"));
    expression_combobox->addItem(tr("afraid"));
    connect(expression_combobox,SIGNAL(currentIndexChanged(int)),this,SLOT(expressionChanged(int)));
    expression_layout->addWidget(expression_combobox);
    expression_box->setLayout(expression_layout);
    left_layout->addWidget(expression_box);

    // auto blink
    QGroupBox* autoblink_box = new QGroupBox(tr("Auto Blink"),left);
    QHBoxLayout* autoblink_layout = new QHBoxLayout();
    speed_slider = new QSlider(Qt::Horizontal,autoblink_box);
    connect(speed_slider,SIGNAL(sliderMoved(int)),this,SLOT(blinkSpeedSliderMoved(int)));
    autoblink_layout->addWidget(speed_slider);
    autoblink_box->setLayout(autoblink_layout);
    left_layout->addWidget(autoblink_box);

    // lookat
    QGroupBox* lookat_box = new QGroupBox(tr("Look At"),left);
    QHBoxLayout* lookat_layout = new QHBoxLayout();
    lookat_combobox = new QComboBox(lookat_box);
    lookat_combobox->addItem(tr("forward"));
    lookat_combobox->addItem(tr("down"));
    lookat_combobox->addItem(tr("up"));
    lookat_combobox->addItem(tr("closest face"));
    lookat_combobox->addItem(tr("furthest face"));
    lookat_combobox->addItem(tr("away (avoid faces)"));
    connect(lookat_combobox,SIGNAL(currentIndexChanged(int)),this,SLOT(lookatChanged(int)));
    lookat_layout->addWidget(lookat_combobox);
    lookat_box->setLayout(lookat_layout);
    left_layout->addWidget(lookat_box);

    // mixer
    QGroupBox* mixer = new QGroupBox(tr("Mixer"),left);
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
    left_layout->addWidget(mixer);

    left->setLayout(left_layout);
    remote_layout->addWidget(left);

    // soundboard
    QWidget* middle = new QGroupBox(tr("Idiom"),remote);
    QVBoxLayout* middle_layout = new QVBoxLayout();
    listwidget = new QListWidget(middle);
    listwidget->setMinimumWidth(200);
    for(QSet<QString>::iterator i = idioms.begin(); i != idioms.end(); i++)
        listwidget->addItem(*i);
    connect(listwidget,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(idiomClicked(QListWidgetItem*)));
    middle_layout->addWidget(listwidget);
    middle->setLayout(middle_layout);
    remote_layout->addWidget(middle);

    // streamer
    QWidget* right = new QGroupBox(tr("A/V Stream"),remote);
    QGridLayout* right_layout = new QGridLayout();
    video = new QVideoWidget(right);
    video->setMinimumSize(640,480);
    mediaplayer->setVideoOutput(video);
    right_layout->addWidget(video,0,0);
    right->setLayout(right_layout);
    remote_layout->addWidget(right);

    remote->setLayout(remote_layout);
    tabs->addTab(remote,tr("Remote Control"));

    // canvas tab
    canvas = new QGraphicsView(model,this);
    tabs->addTab(canvas,tr("Script Canvas"));

    // make tab widget the main widget
    setCentralWidget(tabs);

    setWindowTitle("Untitled.oz[*] - OzTool");
    statusBar()->showMessage("Ready");

    setUndoEnabled(false);
    setRedoEnabled(false);
    save_action->setEnabled(false);
    disconnect_action->setEnabled(false);
    upload_action->setEnabled(false);
    download_action->setEnabled(false);
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
    delete model;
}


void MainWindow::setUndoEnabled(bool state)
{
    undo_action->setEnabled(state);
}


void MainWindow::setRedoEnabled(bool state)
{
    redo_action->setEnabled(state);
}


void MainWindow::touch()
{
    setWindowModified(true);
    saveas_action->setEnabled(true);
    save_action->setEnabled(true);
}


void MainWindow::untouch()
{
    setWindowModified(false);
}


void MainWindow::createActions()
{
    new_action = new QAction(QIcon("new.png"),tr("&New"),this);
    connect(new_action,SIGNAL(triggered()),this,SLOT(fileNew()));
    open_action = new QAction(QIcon("open.png"),tr("&Open"),this);
    connect(open_action,SIGNAL(triggered()),this,SLOT(fileOpen()));
    save_action = new QAction(QIcon("save.png"),tr("&Save"),this);
    connect(save_action,SIGNAL(triggered()),this,SLOT(fileSave()));
    saveas_action = new QAction(tr("Save &As"),this);
    connect(saveas_action,SIGNAL(triggered()),this,SLOT(fileSaveAs()));
    exit_action = new QAction(tr("E&xit"),this);
    connect(exit_action,SIGNAL(triggered()),this,SLOT(fileExit()));

    undo_action = new QAction(QIcon("undo.png"),tr("&Undo"),this);
    connect(undo_action,SIGNAL(triggered()),this,SLOT(editUndo()));
    redo_action = new QAction(QIcon("redo.png"),tr("&Redo"),this);
    connect(redo_action,SIGNAL(triggered()),this,SLOT(editRedo()));

    connect_action = new QAction(QIcon("connect.png"),tr("&Connect"),this);
    connect(connect_action,SIGNAL(triggered()),this,SLOT(robotConnect()));
    disconnect_action = new QAction(QIcon("disconnect.png"),tr("&Disconnect"),this);
    connect(disconnect_action,SIGNAL(triggered()),this,SLOT(robotDisconnect()));
    upload_action = new QAction(QIcon("upload.png"),tr("&Upload to Robot"),this);
    connect(upload_action,SIGNAL(triggered()),this,SLOT(robotUpload()));
    download_action = new QAction(QIcon("download.png"),tr("Down&Load from Robot"),this);
    connect(download_action,SIGNAL(triggered()),this,SLOT(robotDownload()));

    run_action = new QAction(QIcon("run.png"),tr("&Run"),this);
    connect(run_action,SIGNAL(triggered()),this,SLOT(execRun()));
    stop_action = new QAction(QIcon("stop.png"),tr("&Stop"),this);
    connect(stop_action,SIGNAL(triggered()),this,SLOT(execStop()));
    reset_action = new QAction(QIcon("reset.png"),tr("&Reset"),this);
    connect(reset_action,SIGNAL(triggered()),this,SLOT(execReset()));

    options_action = new QAction(tr("&Options"),this);
    connect(options_action,SIGNAL(triggered()),this,SLOT(toolsOptions()));

    overview_action = new QAction(tr("&Overview"),this);
    connect(overview_action,SIGNAL(triggered()),this,SLOT(helpOverview()));
    about_action = new QAction(tr("&About"),this);
    connect(about_action,SIGNAL(triggered()),this,SLOT(helpAbout()));
}


void MainWindow::createMenus()
{
    file_menu = menuBar()->addMenu(tr("&File"));
    file_menu->addAction(new_action);
    file_menu->addAction(open_action);
    file_menu->addSeparator();
    file_menu->addAction(save_action);
    file_menu->addAction(saveas_action);
    file_menu->addSeparator();
    file_menu->addAction(exit_action);

    edit_menu = menuBar()->addMenu(tr("&Edit"));
    edit_menu->addAction(undo_action);
    edit_menu->addAction(redo_action);

    robot_menu = menuBar()->addMenu(tr("&Robot"));
    robot_menu->addAction(connect_action);
    robot_menu->addAction(disconnect_action);
    robot_menu->addSeparator();
    robot_menu->addAction(upload_action);
    robot_menu->addAction(download_action);

    exec_menu = menuBar()->addMenu(tr("&Execute"));
    exec_menu->addAction(run_action);
    exec_menu->addAction(stop_action);
    exec_menu->addAction(reset_action);

    tools_menu = menuBar()->addMenu(tr("&Tools"));
    tools_menu->addAction(options_action);

    help_menu = menuBar()->addMenu(tr("&Help"));
    help_menu->addAction(overview_action);
    help_menu->addSeparator();
    help_menu->addAction(about_action);
}


void MainWindow::createToolBar()
{
    QToolBar* toolbar = addToolBar(tr("Tools"));
    toolbar->addAction(new_action);
    toolbar->addAction(open_action);
    toolbar->addAction(save_action);
    toolbar->addSeparator();
    toolbar->addAction(undo_action);
    toolbar->addAction(redo_action);
    toolbar->addSeparator();
    toolbar->addAction(connect_action);
    toolbar->addAction(disconnect_action);
    toolbar->addAction(upload_action);
    toolbar->addAction(download_action);
    toolbar->addSeparator();
    toolbar->addAction(run_action);
    toolbar->addAction(stop_action);
    toolbar->addAction(reset_action);
}


void MainWindow::saveIfNeeded()
{
    if(isWindowModified())
        if(QMessageBox::question(this,tr("Alert"),tr("Current script has unsaved changes."),QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,QMessageBox::Save) == QMessageBox::Save)
        {
            if(filename.isEmpty())
                fileSaveAs();
            else
                fileSave();
            untouch();
        }
}


void MainWindow::fileNew()
{
    saveIfNeeded();
    model->clear();
}


void MainWindow::fileOpen()
{
    saveIfNeeded();
    QString newname = QFileDialog::getOpenFileName(this,tr("Load Script"),"",tr("Oz Scripts (*.oz);;Any file (*.*)"));
    if(!newname.isNull())
    {
        filename = newname;
        setWindowTitle(filename + "[*] - OzTool");
        model->load(filename);
    }
}


void MainWindow::fileSave()
{
    if(filename.isEmpty())
        fileSaveAs();
    else
    {
        model->save(filename);
        setWindowModified(false);
    }
}


void MainWindow::fileSaveAs()
{
    QString newname = QFileDialog::getSaveFileName(this,tr("Save Script"),"",tr("Oz Scripts (*.oz);;Any file (*.*)"));
    if(!newname.isNull())
    {
        filename = newname;
        setWindowTitle(filename + "[*] - OzTool");
        model->save(filename);
        setWindowModified(false);
    }
}


void MainWindow::fileExit()
{
    saveIfNeeded();
    application.closeAllWindows();
}


void MainWindow::editUndo()
{
    model->undo();
}


void MainWindow::editRedo()
{
    model->redo();
}


void MainWindow::robotConnect()
{
    if(!client.IsConnected())
    {
        ConnectDialog* dialog = new ConnectDialog(this);
        QDialog::DialogCode result = (QDialog::DialogCode)dialog->exec();
        QString name = dialog->name();
        client_ip = 0;
        int port = dialog->port();
        delete dialog;

        if(result == QDialog::Accepted)
        {
            QStringList part = name.split(".");
            if(part.size() == 4)
                client_ip = (part[0].toInt() << 24) | (part[1].toInt() << 16) | (part[2].toInt() << 8) | part[3].toInt();
            else if(name == "localhost")
                client_ip = 0x7F000001;
            else
            {
                QList<QHostAddress> addresses = QHostInfo::fromName(name).addresses();
                if(addresses.size() == 0)
                {
                    QMessageBox::warning(this,"Not Found","Host " + name + " not found");
                    return;
                }
                client_ip = addresses.first().toIPv4Address();
            }
            if(port == 0)
                port = 7331;
            if(client.Connect(client_ip,port))
            {
                //streamer = new QTcpSocket;
                //streamer->connectToHost(name,7332);
                //if(!streamer->waitForConnected(1000))
                //{
                //    QMessageBox::warning(this,"Connection Error","Unable to connect to robot at " + name + ":7332");
                //    client.Disconnect();
                //    streamer->deleteLater();
                //    return;
                //}
                //mediaplayer->setMedia(QMediaContent(),streamer);
                //streamer->waitForReadyRead();
                //mediaplayer->play();
                connect_action->setEnabled(false);
                disconnect_action->setEnabled(true);
                upload_action->setEnabled(true);
                download_action->setEnabled(true);
                speak_lineedit->setEnabled(true);
                speak_button->setEnabled(true);
                volume_slider->setEnabled(true);
                volume_slider->setValue((int)(client.GetVoiceVolume() * 100.0f));
                pitch_slider->setEnabled(true);
                volume_slider->setValue((int)(client.GetVoicePitch() * 100.0f));
                gesture_combobox->setEnabled(true);
                gesture_combobox->setCurrentIndex(0);
                gesture_button->setEnabled(true);
                expression_combobox->setEnabled(true);
                expression_combobox->setCurrentIndex(client.GetExpression());
                speed_slider->setEnabled(true);
                speed_slider->setValue((int)(client.GetAutoBlinkSpeed() * 100.0f));
                lookat_combobox->setEnabled(true);
                lookat_combobox->setCurrentIndex(client.GetLookAt());
                gesture_slider->setEnabled(true);
                gesture_slider->setValue((int)(client.GetGestureWeight() * 100.0f));
                expression_slider->setEnabled(true);
                expression_slider->setValue((int)(client.GetExpressionWeight() * 100.0f));
                lookat_slider->setEnabled(true);
                lookat_slider->setValue((int)(client.GetLookAtWeight() * 100.0f));
                head_slider->setEnabled(true);
                head_slider->setValue((int)(client.GetRandomHeadWeight() * 100.0f));
                eyes_slider->setEnabled(true);
                eyes_slider->setValue((int)(client.GetRandomEyesWeight() * 100.0f));
                face_slider->setEnabled(true);
                face_slider->setValue((int)(client.GetRandomFaceWeight() * 100.0f));
                listwidget->setEnabled(true);
            }
            else
                QMessageBox::warning(this,"Connection Error","Unable to connect to robot at " + name + ":" + QString::number(port));
        }
    }
}


void MainWindow::robotDisconnect()
{
    if(client.IsConnected())
        client.Disconnect();
    disconnect_action->setEnabled(false);
    connect_action->setEnabled(true);
    upload_action->setEnabled(false);
    download_action->setEnabled(false);
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
    streamer->deleteLater();
}


void MainWindow::robotUpload()
{
    // TODO: upload to robot
}


void MainWindow::robotDownload()
{
    // TODO: download from robot
}


void MainWindow::execRun()
{
    // TODO: start execution
}


void MainWindow::execStop()
{
    // TODO: stop execution
}


void MainWindow::execReset()
{
    // TODO: reset
}


void MainWindow::toolsOptions()
{
    // TODO: show options dialog for this model
}


void MainWindow::helpOverview()
{
    // TODO: show modal dialog with html viewer with help stuff
}


void MainWindow::helpAbout()
{
    QMessageBox::about(this,tr("About Oz Tool 0.2"),tr("Oz Robot Puppeteering System is a small and simple humanoid social robot experimenting suite.\nOzTool lets you create and edit visual FSM-like interaction scripts.\nThe current version is 0.2\n(C) Copyrights Germans Media Technology & Services\n"));
}


void MainWindow::speakClicked()
{
    if(client.IsConnected())
        client.Speak(speak_lineedit->text().toUtf8().data());
}


void MainWindow::volumeSliderMoved(int value)
{
    if(client.IsConnected())
        client.VoiceVolume((float)value / 100.0f);
}


void MainWindow::pitchSliderMoved(int value)
{
    if(client.IsConnected())
        client.VoicePitch((float)value / 100.0f);
}


void MainWindow::gestureClicked()
{
    if(client.IsConnected())
        client.Gesture(gesture_combobox->currentIndex());
}


void MainWindow::expressionChanged(int index)
{
    if(client.IsConnected())
        client.Expression(index);
}


void MainWindow::blinkSpeedSliderMoved(int value)
{
    if(client.IsConnected())
        client.AutoBlinkSpeed((float)value / 100.0f);
}


void MainWindow::lookatChanged(int index)
{
    if(client.IsConnected())
        client.LookAt(index);
}


void MainWindow::gestureSliderMoved(int value)
{
    if(client.IsConnected())
        client.GestureWeight((float)value / 100.0f);
}


void MainWindow::expressionSliderMoved(int value)
{
    if(client.IsConnected())
        client.ExpressionWeight((float)value / 100.0f);
}


void MainWindow::lookatSliderMoved(int value)
{
    if(client.IsConnected())
        client.LookAtWeight((float)value / 100.0f);
}


void MainWindow::headSliderMoved(int value)
{
    if(client.IsConnected())
        client.RandomHeadWeight((float)value / 100.0f);
}


void MainWindow::eyesSliderMoved(int value)
{
    if(client.IsConnected())
        client.RandomEyesWeight((float)value / 100.0f);
}


void MainWindow::faceSliderMoved(int value)
{
    if(client.IsConnected())
        client.RandomFaceWeight((float)value / 100.0f);
}


void MainWindow::idiomClicked(QListWidgetItem* item)
{
    if(client.IsConnected())
        client.Speak(item->text().toUtf8().data());
}
