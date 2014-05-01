#ifndef NEWTRANSITIONCOMMAND_H
#define NEWTRANSITIONCOMMAND_H

#include "Command.h"
#include "Transition.h"
#include <QPointF>


class NewTransitionCommand : public Command
{
public:
    NewTransitionCommand(Model& amodel,Transition* atransition);
    virtual void Apply();
    virtual void Unapply();

private:
    Transition* transition;
};


#endif
