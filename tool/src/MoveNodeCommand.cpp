#include "MoveNodeCommand.h"


MoveNodeCommand::MoveNodeCommand(Model& amodel,Node* anode,const QPointF& apos_before,const QPointF& apos_after) : Command(amodel),node(anode),pos_before(apos_before),pos_after(apos_after)
{
}


void MoveNodeCommand::Apply()
{
    node->setPos(pos_before);
}


void MoveNodeCommand::Unapply()
{
    node->setPos(pos_after);
}
