#include "ConnectDialog.h"
#include <QGridLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QHostInfo>


// TODO: add real history list to the combobox


ConnectDialog::ConnectDialog(QWidget* parent) : QDialog(parent)
{
    QGridLayout* layout = new QGridLayout();
    layout->addWidget(new QLabel(tr("Robot IP address"),this),0,0,Qt::AlignRight);
    combobox = new QComboBox(this);
    combobox->addItem(tr("localhost"));
    combobox->addItem(tr("145.108.224.19"));
    combobox->addItem(tr("145.108.224.45"));
    combobox->addItem(tr("145.108.224.50"));
    combobox->addItem(tr("145.108.224.52"));
    combobox->addItem(tr("192.168.1.4"));
    combobox->setEditable(true);
    layout->addWidget(combobox,0,1);
    QGridLayout* buttons = new QGridLayout();
    QPushButton* connect_button = new QPushButton(tr("Connect"),this);
    connect(connect_button,SIGNAL(clicked()),this,SLOT(connectClicked()));
    buttons->addWidget(connect_button,0,0);
    QPushButton* cancel_button = new QPushButton(tr("Cancel"),this);
    connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancelClicked()));
    buttons->addWidget(cancel_button,0,1);
    layout->addLayout(buttons,1,0,1,2,Qt::AlignRight);
    setLayout(layout);
    setModal(true);
    setWindowTitle(tr("Connect to Robot"));
    show();
}


void ConnectDialog::connectClicked()
{
    done(QDialog::Accepted);
}


void ConnectDialog::cancelClicked()
{
    done(QDialog::Rejected);
}


int ConnectDialog::port()
{
    QStringList parts = combobox->currentText().split(":");
    target_name = parts[0];
    if(parts.size() == 1)
        target_port = 0;
    else
        target_port = parts[1].toInt();
    return target_port;
}


QString ConnectDialog::name()
{
    QStringList parts = combobox->currentText().split(":");
    target_name = parts[0];
    if(parts.size() == 1)
        target_port = 0;
    else
        target_port = parts[1].toInt();
    return target_name;
}
