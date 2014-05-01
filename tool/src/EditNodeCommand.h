#ifndef EDITNODECOMMAND_H
#define EDITNODECOMMAND_H

#include "Command.h"
#include "Node.h"


class EditNodeCommand : public Command
{
public:
    EditNodeCommand(Model& amodel,Node* anode,QString& aentry_text_before,QString& aentry_text_after);
    virtual void Apply();
    virtual void Unapply();

private:
    Node* node;
    QString entry_text_before,entry_text_after;
};


#endif
