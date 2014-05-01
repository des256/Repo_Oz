#ifndef CONNECTDIALOG_H
#define CONNECTDIALOG_H

#include <QDialog>
#include <QComboBox>


class ConnectDialog : public QDialog
{
    Q_OBJECT

public:
    ConnectDialog(QWidget* parent = 0);

    int port();
    QString name();

public slots:
    void connectClicked();
    void cancelClicked();

private:
    QComboBox* combobox;
    QString target_name;
    int target_port;
};


#endif // CONNECTDIALOG_H
