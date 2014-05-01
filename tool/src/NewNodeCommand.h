#ifndef NEWNODECOMMAND_H
#define NEWNODECOMMAND_H

#include "Command.h"
#include "Node.h"
#include <QPointF>


class NewNodeCommand : public Command
{
public:
    NewNodeCommand(Model& amodel,Node* anode);
    virtual void Apply();
    virtual void Unapply();

private:
    Node* node;
};


#endif
