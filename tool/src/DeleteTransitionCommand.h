#ifndef DELETETRANSITIONCOMMAND_H
#define DELETETRANSITIONCOMMAND_H

#include "Command.h"
#include "Transition.h"


class DeleteTransitionCommand : public Command
{
public:
    DeleteTransitionCommand(Model& amodel,Transition* atransition);
    virtual void Apply();
    virtual void Unapply();

private:
    Transition* transition;
};


#endif
