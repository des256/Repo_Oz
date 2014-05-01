#include "Node.h"
#include "Model.h"
#include "NodeDialog.h"
#include "TransitionDialog.h"
#include "MoveNodeCommand.h"
#include <QPainter>
#include <QMenu>
#include <QGraphicsSceneContextMenuEvent>
#include <QTextOption>


Node::Node(Model& amodel,const QPointF& apos,const QString& atext) : QGraphicsItem(),model(amodel),entry_text(atext),state(NODE_IDLE),offset(),start_pos()
{
    setPos(apos);
}


void Node::setEntryText(const QString& atext)
{
    entry_text = atext;
    update();
}


QString Node::entryText()
{
    return entry_text;
}


QRectF Node::boundingRect() const
{
    return QRectF(-80.5,-40.5,161.0,81.0);
}


void Node::paint(QPainter* painter,const QStyleOptionGraphicsItem* option,QWidget* widget)
{
    switch(state)
    {
        case NODE_IDLE:
            painter->setBrush(Qt::lightGray);
            painter->setPen(Qt::lightGray);
            painter->drawRect(-80,-40,160,80);
            painter->setPen(Qt::black);
            painter->drawText(QRectF(-80,-40,160,80),Qt::AlignCenter,entry_text);
            break;

        case NODE_MOVING:
            painter->setBrush(Qt::NoBrush);
            painter->setPen(Qt::yellow);
            painter->drawRect(-80,-40,160,80);
            break;
    }

}


void Node::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    QMenu menu;
    QAction* edit_action = menu.addAction("Edit");
    QAction* transition_action = menu.addAction("New Transition");
    menu.addSeparator();
    QAction* delete_action = menu.addAction("Delete");
    QAction* action = menu.exec(event->screenPos());
    if(action == edit_action)
        model.editNode(this);
    else if(action == transition_action)
    {
        TransitionDialog* dialog = new TransitionDialog(0);
        if(dialog->exec() == QDialog::Accepted)
            model.newTransition(QPointF(),dialog->conditionText(),dialog->transitText(),this,0);
    }
    else if(action == delete_action)
        model.deleteNode(this);
}


void Node::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if(state == NODE_IDLE)
        if(event->button() == Qt::LeftButton)
        {
            state = NODE_MOVING;
            start_pos = pos();
            offset = event->scenePos() - start_pos;
            grabMouse();
            update();
        }
}


void Node::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if(state == NODE_MOVING)
    {
        ungrabMouse();
        model.pushUndo(new MoveNodeCommand(model,this,start_pos,event->scenePos() - offset));
        state = NODE_IDLE;
        update();
    }
}


void Node::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if(state == NODE_MOVING)
        setPos(event->scenePos() - offset);
}


void Node::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    model.editNode(this);
}
