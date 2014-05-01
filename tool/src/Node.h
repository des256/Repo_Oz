#ifndef NODE_H
#define NODE_H

#include <QGraphicsItem>


enum
{
    NODE_IDLE = 0,
    NODE_MOVING
};


class Model;
class Node : public QGraphicsItem
{
public:
    Node(Model& amodel,const QPointF& apos,const QString& atext);

    void setEntryText(const QString& text);
    QString entryText();

    virtual QRectF boundingRect() const;
    virtual void paint(QPainter* painter,const QStyleOptionGraphicsItem* option,QWidget* widget);

    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent* event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);

private:
    Model& model;
    QString entry_text;
    int state;
    QPointF offset;
    QPointF start_pos;
};


#endif
