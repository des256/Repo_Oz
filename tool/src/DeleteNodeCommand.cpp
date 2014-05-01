#include "DeleteNodeCommand.h"
#include "Model.h"


DeleteNodeCommand::DeleteNodeCommand(Model& amodel,Node* anode) : Command(amodel),node(anode)
{
}


void DeleteNodeCommand::Apply()
{
    model.unlinkNode(node);
}


void DeleteNodeCommand::Unapply()
{
    model.linkNode(node);
}
