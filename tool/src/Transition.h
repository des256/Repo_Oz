#ifndef TRANSITION_H
#define TRANSITION_H

#include "Node.h"
#include <QGraphicsItem>


enum
{
    TRANSITION_IDLE = 0,
    TRANSITION_MOVING,
    TRANSITION_CONNECTING
};


class Model;
class Transition : public QGraphicsItem
{
public:
    Transition(Model& amodel,const QPointF& apos,const QString& acondition_text,const QString& atransit_text,Node* afrom_node,Node* ato_node);
    void setFromNode(Node* afrom_node);
    Node* fromNode();
    void setToNode(Node* ato_node);
    Node* toNode();
    void setConditionText(const QString& acondition_text);
    QString conditionText();
    void setTransitText(const QString& atransit_text);
    QString transitText();

    virtual QRectF boundingRect() const;
    virtual void paint(QPainter* painter,const QStyleOptionGraphicsItem* option,QWidget* widget);

    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent* event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);

private:
    Model& model;
    QString condition_text;
    QString transit_text;
    Node* from_node;
    Node* to_node;
    int state;
    QPointF offset;
    QPointF start_pos;
    QPointF end_pos;
};


#endif
