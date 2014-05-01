#include "NodeDialog.h"
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>


NodeDialog::NodeDialog(Node* anode) : QDialog(0),node(anode)
{
    QGridLayout* layout = new QGridLayout();
    layout->addWidget(new QLabel(tr("Entry text"),this),0,0,Qt::AlignRight);
    entry_text_lineedit = new QLineEdit(this);
    entry_text_lineedit->setToolTip(tr("What the robot always says\nwhen entering this state."));
    layout->addWidget(entry_text_lineedit,0,1);
    QGridLayout* buttons = new QGridLayout();
    QPushButton* ok_button = new QPushButton(tr("Ok"),this);
    connect(ok_button,SIGNAL(clicked()),this,SLOT(okClicked()));
    buttons->addWidget(ok_button,0,0);
    QPushButton* cancel_button = new QPushButton(tr("Cancel"),this);
    connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancelClicked()));
    buttons->addWidget(cancel_button,0,1);
    layout->addLayout(buttons,1,0,1,2,Qt::AlignRight);
    setLayout(layout);
    setModal(true);
    if(node)
    {
        entry_text_lineedit->setText(node->entryText());
        setWindowTitle(tr("Edit Node"));
    }
    else
        setWindowTitle(tr("New Node"));
    show();
}


QString NodeDialog::entryText()
{
    return entry_text_lineedit->text();
}


void NodeDialog::okClicked()
{
    if(node)
        node->setEntryText(entry_text_lineedit->text());
    done(QDialog::Accepted);
}


void NodeDialog::cancelClicked()
{
    done(QDialog::Rejected);
}
