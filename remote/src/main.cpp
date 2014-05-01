#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    application.setApplicationName("OzTool");
    application.setOrganizationName("Germans Media Technology & Services");
    MainWindow mainwindow(application);
    mainwindow.resize(640,480);
    mainwindow.show();
    return application.exec();
}
