#include "TransitionDialog.h"
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>


TransitionDialog::TransitionDialog(Transition* atransition) : QDialog(0),transition(atransition)
{
    QGridLayout* layout = new QGridLayout();
    layout->addWidget(new QLabel(tr("Condition"),this),0,0,Qt::AlignRight);
    condition_text_lineedit = new QLineEdit(this);
    condition_text_lineedit->setToolTip(tr("What the user says to\ntrigger this transition."));
    layout->addWidget(condition_text_lineedit,0,1);
    layout->addWidget(new QLabel(tr("Transit text"),this),1,0,Qt::AlignRight);
    transit_text_lineedit = new QLineEdit(this);
    transit_text_lineedit->setToolTip(tr("What the robot says when\ntaking this transition."));
    layout->addWidget(transit_text_lineedit,1,1);
    QGridLayout* buttons = new QGridLayout();
    QPushButton* ok_button = new QPushButton(tr("Ok"),this);
    connect(ok_button,SIGNAL(clicked()),this,SLOT(okClicked()));
    buttons->addWidget(ok_button,0,0);
    QPushButton* cancel_button = new QPushButton(tr("Cancel"),this);
    connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancelClicked()));
    buttons->addWidget(cancel_button,0,1);
    layout->addLayout(buttons,2,0,1,2,Qt::AlignRight);
    setLayout(layout);
    setModal(true);
    if(transition)
    {
        condition_text_lineedit->setText(transition->conditionText());
        transit_text_lineedit->setText(transition->transitText());
        setWindowTitle(tr("Edit Transition"));
    }
    else
        setWindowTitle(tr("New Transition"));
    show();
}


QString TransitionDialog::conditionText()
{
    return condition_text_lineedit->text();
}


QString TransitionDialog::transitText()
{
    return transit_text_lineedit->text();
}


void TransitionDialog::okClicked()
{
    done(QDialog::Accepted);
}


void TransitionDialog::cancelClicked()
{
    done(QDialog::Rejected);
}
