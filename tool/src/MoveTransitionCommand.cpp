#include "MoveTransitionCommand.h"


MoveTransitionCommand::MoveTransitionCommand(Model& amodel,Transition* atransition,const QPointF& apos_before,const QPointF& apos_after) : Command(amodel),transition(atransition),pos_before(apos_before),pos_after(apos_after)
{
}


void MoveTransitionCommand::Apply()
{
    transition->setPos(pos_after);
}


void MoveTransitionCommand::Unapply()
{
    transition->setPos(pos_before);
}
