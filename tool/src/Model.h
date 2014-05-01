#ifndef MODEL_H
#define MODEL_H

#include "Node.h"
#include "Transition.h"
#include "Command.h"
#include <QSet>
#include <QStack>
#include <QGraphicsScene>


class MainWindow;
class Model : public QGraphicsScene
{
public:
    Model(MainWindow& amainwindow);
    virtual ~Model();

    void clear();
    void save(const QString& filename);
    void load(const QString& filename);

    void pushUndo(Command* command);
    void undo();
    void redo();

    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent* event);

    Node* newNode(const QPointF& pos,const QString& entry);
    void deleteNode(Node* node);
    void editNode(Node* node);

    Transition* newTransition(const QPointF& pos,const QString& condition_text,const QString& transit_text,Node* from_node,Node* to_node);
    void deleteTransition(Transition* transition);
    void editTransition(Transition* transition);

    void linkNode(Node* node);
    void unlinkNode(Node* node);

    void linkTransition(Transition* transition);
    void unlinkTransition(Transition* transition);

private:
    QStack<Command*> undo_stack;
    QStack<Command*> redo_stack;
    MainWindow& mainwindow;
};


#endif // MODEL_H
