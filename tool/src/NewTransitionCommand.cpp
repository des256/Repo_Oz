#include "NewTransitionCommand.h"
#include "Model.h"


NewTransitionCommand::NewTransitionCommand(Model& amodel,Transition* atransition) : Command(amodel),transition(atransition)
{
}


void NewTransitionCommand::Apply()
{
    model.linkTransition(transition);
}


void NewTransitionCommand::Unapply()
{
    model.unlinkTransition(transition);
}
