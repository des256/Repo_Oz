#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Model.h"
#include "../Client1.h"
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


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QApplication& aapplication,QWidget* parent = 0);
    ~MainWindow();

    void setUndoEnabled(bool state);
    void setRedoEnabled(bool state);
    void touch();
    void untouch();

public slots:
    void fileNew();
    void fileOpen();
    void fileSave();
    void fileSaveAs();
    void fileExit();

    void editUndo();
    void editRedo();

    void robotConnect();
    void robotDisconnect();
    void robotUpload();
    void robotDownload();

    void execRun();
    void execStop();
    void execReset();

    void toolsOptions();

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

private:
    void createActions();
    void createMenus();
    void createToolBar();
    void saveIfNeeded();

    QApplication& application;

    oz::Client1 client;

    QString filename;
    Model* model;
    QGraphicsView* canvas;

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

    QMediaPlayer* mediaplayer;
    QVideoWidget* video;
    QTcpSocket* streamer;

    QSet<QString> idioms;

    QAction* new_action;
    QAction* open_action;
    QAction* save_action;
    QAction* saveas_action;
    QAction* exit_action;

    QAction* undo_action;
    QAction* redo_action;

    QAction* connect_action;
    QAction* disconnect_action;

    QAction* upload_action;
    QAction* download_action;

    QAction* run_action;
    QAction* stop_action;
    QAction* reset_action;

    QAction* options_action;

    QAction* overview_action;
    QAction* about_action;

    QMenu* file_menu;
    QMenu* edit_menu;
    QMenu* robot_menu;
    QMenu* exec_menu;
    QMenu* tools_menu;
    QMenu* help_menu;
};


#endif // MAINWINDOW_H
