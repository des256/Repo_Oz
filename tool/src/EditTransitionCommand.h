#ifndef EDITTRANSITIONCOMMAND_H
#define EDITTRANSITIONCOMMAND_H

#include "Command.h"
#include "Transition.h"


class EditTransitionCommand : public Command
{
public:
    EditTransitionCommand(Model& amodel,Transition* atransition,const QString& acondition_text_before,const QString& atransit_text_before,const QString& acondition_text_after,const QString& atransit_text_after);
    virtual void Apply();
    virtual void Unapply();

private:
    Transition* transition;
    QString condition_text_before,transit_text_before;
    QString condition_text_after,transit_text_after;
};


#endif
