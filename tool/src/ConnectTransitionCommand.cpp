#include "ConnectTransitionCommand.h"
#include "Model.h"


ConnectTransitionCommand::ConnectTransitionCommand(Model& amodel,Transition* atransition,Node* ato_node_before,Node* ato_node_after) : Command(amodel),transition(atransition),to_node_before(ato_node_before),to_node_after(ato_node_after)
{
}


void ConnectTransitionCommand::Apply()
{
    transition->setToNode(to_node_after);
}


void ConnectTransitionCommand::Unapply()
{
    transition->setToNode(to_node_before);
}
