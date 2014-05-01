#ifndef MOVETRANSITIONCOMMAND_H
#define MOVETRANSITIONCOMMAND_H

#include "Command.h"
#include "Transition.h"


class MoveTransitionCommand : public Command
{
public:
    MoveTransitionCommand(Model& amodel,Transition* atransition,const QPointF& apos_before,const QPointF& apos_after);

    virtual void Apply();
    virtual void Unapply();

private:
    Transition* transition;
    QPointF pos_before,pos_after;
};


#endif
