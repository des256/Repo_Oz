#ifndef MOVENODECOMMAND_H
#define MOVENODECOMMAND_H

#include "Command.h"
#include "Node.h"


class MoveNodeCommand : public Command
{
public:
    MoveNodeCommand(Model& amodel,Node* anode,const QPointF& apos_before,const QPointF& apos_after);

    virtual void Apply();
    virtual void Unapply();

private:
    Node* node;
    QPointF pos_before,pos_after;
};


#endif
