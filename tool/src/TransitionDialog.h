#ifndef TRANSITIONDIALOG_H
#define TRANSITIONDIALOG_H

#include "Transition.h"
#include <QDialog>
#include <QLineEdit>


class TransitionDialog : public QDialog
{
    Q_OBJECT

public:
    TransitionDialog(Transition* atransition);
    QString conditionText();
    QString transitText();

public slots:
    void okClicked();
    void cancelClicked();

private:
    Transition* transition;
    QLineEdit* condition_text_lineedit;
    QLineEdit* transit_text_lineedit;
};


#endif  // TRANSITIONDIALOG_H
