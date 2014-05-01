#ifndef COMMAND_H
#define COMMAND_H


class Model;
class Command
{
public:
    Command(Model& amodel) : model(amodel) { };
    virtual ~Command() { };

    virtual void Apply() = 0;
    virtual void Unapply() = 0;

protected:
    Model& model;
};


#endif
