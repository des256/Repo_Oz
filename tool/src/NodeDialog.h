#ifndef NODEDIALOG_H
#define NODEDIALOG_H

#include "Node.h"
#include <QDialog>
#include <QLineEdit>


class NodeDialog : public QDialog
{
    Q_OBJECT

public:
    NodeDialog(Node* anode);
    QString entryText();

public slots:
    void okClicked();
    void cancelClicked();

private:
    Node* node;
    QLineEdit* entry_text_lineedit;
};


#endif  // NODEDIALOG_H
