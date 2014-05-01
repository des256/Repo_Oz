#include "NewNodeCommand.h"
#include "Model.h"


NewNodeCommand::NewNodeCommand(Model& amodel,Node* anode) : Command(amodel),node(anode)
{
}


void NewNodeCommand::Apply()
{
    model.linkNode(node);
}


void NewNodeCommand::Unapply()
{
    model.unlinkNode(node);
}
