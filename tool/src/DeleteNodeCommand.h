#ifndef DELETENODECOMMAND_H
#define DELETENODECOMMAND_H

#include "Command.h"
#include "Node.h"


class DeleteNodeCommand : public Command
{
public:
    DeleteNodeCommand(Model& amodel,Node* anode);
    virtual void Apply();
    virtual void Unapply();

private:
    Node* node;
};


#endif
