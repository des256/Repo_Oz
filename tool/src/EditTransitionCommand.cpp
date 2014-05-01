#include "EditTransitionCommand.h"


EditTransitionCommand::EditTransitionCommand(Model& amodel,Transition* atransition,const QString& acondition_text_before,const QString& atransit_text_before,const QString& acondition_text_after,const QString& atransit_text_after) : Command(amodel),transition(atransition),condition_text_before(acondition_text_before),transit_text_before(atransit_text_before),condition_text_after(acondition_text_after),transit_text_after(atransit_text_after)
{
}


void EditTransitionCommand::Apply()
{
    transition->setConditionText(condition_text_after);
    transition->setTransitText(transit_text_after);
}


void EditTransitionCommand::Unapply()
{
    transition->setConditionText(condition_text_before);
    transition->setTransitText(transit_text_before);
}
