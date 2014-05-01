#include "Transition.h"
#include "Model.h"
#include "TransitionDialog.h"
#include "MoveTransitionCommand.h"
#include <QPainter>
#include <QMenu>
#include <QGraphicsSceneContextMenuEvent>
#include <QTextOption>


Transition::Transition(Model& amodel,const QPointF& apos,const QString& acondition_text,const QString& atransit_text,Node* afrom_node,Node* ato_node) : QGraphicsItem(),model(amodel),condition_text(acondition_text),transit_text(atransit_text),from_node(afrom_node),to_node(ato_node),state(TRANSITION_IDLE),offset(),start_pos(),end_pos()
{
    setPos(apos);
    setToolTip(acondition_text);
}


void Transition::setFromNode(Node* afrom_node)
{
    from_node = afrom_node;
}


Node* Transition::fromNode()
{
    return from_node;
}


void Transition::setToNode(Node* ato_node)
{
    to_node = ato_node;
}


Node* Transition::toNode()
{
    return to_node;
}


void Transition::setConditionText(const QString& acondition_text)
{
    condition_text = acondition_text;
    setToolTip(acondition_text);
}


QString Transition::conditionText()
{
    return condition_text;
}


void Transition::setTransitText(const QString& atransit_text)
{
    transit_text = atransit_text;
}


QString Transition::transitText()
{
    return transit_text;
}

QRectF Transition::boundingRect() const
{
    return QRectF(-5.5,-5.5,11,11);
}


void Transition::paint(QPainter* painter,const QStyleOptionGraphicsItem* option,QWidget* widget)
{
    switch(state)
    {
        case TRANSITION_IDLE:
            if(to_node)
            {
                painter->setPen(Qt::lightGray);
                QPointF pos = mapFromItem(to_node,QPointF());
                painter->drawLine(QPointF(),pos);
            }
            painter->setBrush(Qt::darkBlue);
            painter->setPen(Qt::darkBlue);
            painter->drawEllipse(-5,-5,10,10);
            break;

        case TRANSITION_MOVING:
            painter->setBrush(Qt::NoBrush);
            painter->setPen(Qt::yellow);
            painter->drawEllipse(-5,-5,10,10);
            if(to_node)
            {
                QPointF pos = mapFromItem(to_node,QPointF());
                painter->drawLine(QPointF(),pos);
            }
            break;

        case TRANSITION_CONNECTING:
            painter->setPen(Qt::yellow);
            painter->drawLine(QPointF(),mapFromScene(end_pos));
            painter->setBrush(Qt::darkBlue);
            painter->setPen(Qt::darkBlue);
            painter->drawEllipse(-5,-5,10,10);
            break;
    }
}


void Transition::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    QMenu menu;
    QAction* edit_action = menu.addAction("Edit");
    menu.addSeparator();
    QAction* delete_action = menu.addAction("Delete");
    QAction* action = menu.exec(event->screenPos());
    if(action == edit_action)
        model.editTransition(this);
    else if(action == delete_action)
        model.deleteTransition(this);
}


void Transition::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if(state == TRANSITION_IDLE)
        switch(event->button())
        {
            case Qt::LeftButton:
                state = TRANSITION_MOVING;
                start_pos = pos();
                offset = event->scenePos() - start_pos;
                grabMouse();
                update();
                break;

            case Qt::RightButton:
                state = TRANSITION_CONNECTING;
                start_pos = pos();
                offset = event->scenePos() - start_pos;
                grabMouse();
                update();
                break;
        }
}


void Transition::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    switch(state)
    {
        case TRANSITION_MOVING:
            ungrabMouse();
            model.pushUndo(new MoveTransitionCommand(model,this,start_pos,event->scenePos() - offset));
            state = NODE_IDLE;
            update();
            break;

        case TRANSITION_CONNECTING:
            ungrabMouse();
            // TODO: add connect command with found node
            state = NODE_IDLE;
            update();
            event->accept();
            break;
    }
}


void Transition::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    switch(state)
    {
        case TRANSITION_MOVING:
            setPos(event->scenePos() - offset);
            break;

        case TRANSITION_CONNECTING:
            end_pos = event->scenePos() - offset;
            update();
            break;
    }
}


void Transition::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    model.editTransition(this);
}
