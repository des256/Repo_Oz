#include "DeleteTransitionCommand.h"
#include "Model.h"


DeleteTransitionCommand::DeleteTransitionCommand(Model& amodel,Transition* atransition) : Command(amodel),transition(atransition)
{
}


void DeleteTransitionCommand::Apply()
{
    model.unlinkTransition(transition);
}


void DeleteTransitionCommand::Unapply()
{
    model.linkTransition(transition);
}
