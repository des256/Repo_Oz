#ifndef CONNECTTRANSITIONCOMMAND_H
#define CONNECTTRANSITIONCOMMAND_H

#include "Command.h"
#include "Transition.h"


class ConnectTransitionCommand : public Command
{
public:
    ConnectTransitionCommand(Model& amodel,Transition* atransition,Node* ato_node_before,Node* ato_node_after);
    virtual void Apply();
    virtual void Unapply();

private:
    Transition* transition;
    Node* to_node_before,* to_node_after;
};


#endif
