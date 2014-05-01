#include "EditNodeCommand.h"


EditNodeCommand::EditNodeCommand(Model& amodel,Node* anode,QString& aentry_text_before,QString& aentry_text_after) : Command(amodel),node(anode),entry_text_before(aentry_text_before),entry_text_after(aentry_text_after)
{
}


void EditNodeCommand::Apply()
{
    node->setEntryText(entry_text_after);
}


void EditNodeCommand::Unapply()
{
    node->setEntryText(entry_text_before);
}
