#include "Model.h"
#include "MainWindow.h"
#include "NodeDialog.h"
#include "TransitionDialog.h"
#include "NewNodeCommand.h"
#include "DeleteNodeCommand.h"
#include "EditNodeCommand.h"
#include "NewTransitionCommand.h"
#include "DeleteTransitionCommand.h"
#include "EditTransitionCommand.h"
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QMenu>
#include <QGraphicsSceneContextMenuEvent>


Model::Model(MainWindow& amainwindow) : QGraphicsScene(),undo_stack(),redo_stack(),mainwindow(amainwindow)
{
    setSceneRect(0,0,100,100);
    setBackgroundBrush(Qt::darkGray);
}


Model::~Model()
{
    clear();
}


void Model::clear()
{
    // clear undo/redo stacks
    while(!redo_stack.empty())
        delete redo_stack.pop();
    while(!undo_stack.empty())
        delete undo_stack.pop();
    mainwindow.setUndoEnabled(false);
    mainwindow.setRedoEnabled(false);

    // clear the objects
    QGraphicsScene::clear();
}


void Model::save(const QString& filename)
{
    QMap<Node*,int> node_ids;
    int id = 1;

    QFile file(filename);
    file.open(QIODevice::WriteOnly);
    QXmlStreamWriter xml(&file);
    xml.writeStartDocument();
    xml.writeStartElement("ozscript");
    QList<QGraphicsItem*> all_items = items();
    for(QList<QGraphicsItem*>::iterator i = all_items.begin(); i != all_items.end(); i++)
        if((*i)->isVisible())
        {
            Node* node = dynamic_cast<Node*>(*i);
            if(node)
            {
                xml.writeStartElement("node");
                QPointF pos = node->pos();
                xml.writeAttribute("id",QString::number(id));
                xml.writeAttribute("x",QString::number(pos.x()));
                xml.writeAttribute("y",QString::number(pos.y()));
                xml.writeAttribute("entry",node->entryText());
                xml.writeEndElement();
                node_ids[node] = id;
                id++;
            }
        }
    for(QList<QGraphicsItem*>::iterator i = all_items.begin(); i != all_items.end(); i++)
        if((*i)->isVisible())
        {
            Transition* transition = dynamic_cast<Transition*>(*i);
            if(transition)
            {
                xml.writeStartElement("transition");
                QPointF pos = transition->pos();
                xml.writeAttribute("x",QString::number(pos.x()));
                xml.writeAttribute("y",QString::number(pos.y()));
                xml.writeAttribute("condition",transition->conditionText());
                xml.writeAttribute("transit",transition->transitText());
                xml.writeAttribute("from",QString::number(node_ids[transition->fromNode()]));
                xml.writeAttribute("to",QString::number(node_ids[transition->toNode()]));
                xml.writeEndElement();
            }
        }
    xml.writeEndElement();
    xml.writeEndDocument();
    file.close();
    mainwindow.untouch();
}


void Model::load(const QString& filename)
{
    QMap<int,Node*> node_ids;
    node_ids[0] = 0;

    clear();
    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    QXmlStreamReader xml(&file);
    while(!xml.atEnd())
    {
        QXmlStreamReader::TokenType type = xml.readNext();
        if(type == QXmlStreamReader::StartElement)
        {
            if(xml.name() == "node")
            {
                QPointF pos;
                QString entry = "";
                int id;
                QXmlStreamAttributes attributes = xml.attributes();
                if(attributes.hasAttribute("x"))
                    pos.setX(attributes.value("x").toDouble());
                if(attributes.hasAttribute("y"))
                    pos.setY(attributes.value("y").toDouble());
                if(attributes.hasAttribute("entry"))
                    entry = attributes.value("entry").toString();
                if(attributes.hasAttribute("id"))
                    id = attributes.value("id").toInt();
                Node* node = new Node(*this,pos,entry);
                addItem(node);
                node_ids[id] = node;
            }
            else if(xml.name() == "transition")
            {
                QPointF pos;
                QString condition = "";
                QString transit = "";
                int from_id = 0;
                int to_id = 0;
                QXmlStreamAttributes attributes = xml.attributes();
                if(attributes.hasAttribute("x"))
                    pos.setX(attributes.value("x").toDouble());
                if(attributes.hasAttribute("y"))
                    pos.setY(attributes.value("y").toDouble());
                if(attributes.hasAttribute("from"))
                    from_id = attributes.value("from").toInt();
                if(attributes.hasAttribute("to"))
                    to_id = attributes.value("to").toInt();
                if(attributes.hasAttribute("condition"))
                    condition = attributes.value("condition").toString();
                if(attributes.hasAttribute("transit"))
                    transit = attributes.value("transit").toString();
                Transition* transition = new Transition(*this,pos,condition,transit,node_ids[from_id],node_ids[to_id]);
                addItem(transition);
            }
        }
    }
    if(xml.hasError())
    {
        // TODO: process error
    }
    file.close();
    mainwindow.setWindowModified(false);
}


void Model::pushUndo(Command* command)
{
    undo_stack.push(command);
    while(!redo_stack.empty())
        delete redo_stack.pop();
    mainwindow.setRedoEnabled(false);
    mainwindow.setUndoEnabled(true);
}


void Model::undo()
{
    if(!undo_stack.empty())
    {
        Command* command = undo_stack.pop();
        if(undo_stack.empty())
            mainwindow.setUndoEnabled(false);
        command->Unapply();
        redo_stack.push(command);
        mainwindow.setRedoEnabled(true);
    }
}


void Model::redo()
{
    if(!redo_stack.empty())
    {
        Command* command = redo_stack.pop();
        if(redo_stack.empty())
            mainwindow.setRedoEnabled(false);
        command->Apply();
        undo_stack.push(command);
        mainwindow.setUndoEnabled(true);
    }
}


void Model::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    QGraphicsItem* item = itemAt(event->scenePos(),QTransform());
    if(item)
        QGraphicsScene::contextMenuEvent(event);
    else
    {
        QMenu menu;
        QAction* new_node_action = menu.addAction("New Node");
        QAction* new_transition_action = menu.addAction("New Transition");
        QAction* action = menu.exec(event->screenPos());
        if(action == new_node_action)
        {
            NodeDialog* dialog = new NodeDialog(0);
            if(dialog->exec() == QDialog::Accepted)
                newNode(event->scenePos(),dialog->entryText());
        }
        else if(action == new_transition_action)
        {
            TransitionDialog* dialog = new TransitionDialog(0);
            if(dialog->exec() == QDialog::Accepted)
                newTransition(event->scenePos(),dialog->conditionText(),dialog->transitText(),0,0);
        }
    }
}


Node* Model::newNode(const QPointF& pos,const QString& entry)
{
    // create new node
    Node* node = new Node(*this,pos,entry);
    addItem(node);

    // push undo command
    pushUndo(new NewNodeCommand(*this,node));

    // link into model
    linkNode(node);

    // and indicate dirty
    mainwindow.touch();

    return node;
}


void Model::deleteNode(Node* node)
{
    // push undo command
    pushUndo(new DeleteNodeCommand(*this,node));

    // unlink from model
    unlinkNode(node);

    // and indicate dirty
    mainwindow.touch();
}


void Model::editNode(Node* node)
{
    // save old parameters
    QString entry_text_before = node->entryText();

    // open the dialog
    NodeDialog* dialog = new NodeDialog(node);

    // push undo command if something actually changed
    if(dialog->exec() == QDialog::Accepted)
        pushUndo(new EditNodeCommand(*this,node,entry_text_before,node->entryText()));
}


Transition* Model::newTransition(const QPointF& pos,const QString& condition_text,const QString& transit_text,Node* from_node,Node* to_node)
{
    // create new transition
    Transition* transition = new Transition(*this,pos,condition_text,transit_text,from_node,to_node);
    addItem(transition);

    // push undo command
    pushUndo(new NewTransitionCommand(*this,transition));

    // link into model
    linkTransition(transition);

    // and indicate dirty
    mainwindow.touch();

    return transition;
}


void Model::deleteTransition(Transition* transition)
{
    // push undo command
    pushUndo(new DeleteTransitionCommand(*this,transition));

    // unlink from model
    unlinkTransition(transition);

    // and indicate dirty
    mainwindow.touch();
}


void Model::editTransition(Transition* transition)
{
    // save old parameters
    QString condition_text_before = transition->conditionText();
    QString transit_text_before = transition->transitText();

    // open the dialog
    TransitionDialog* dialog = new TransitionDialog(transition);

    // push undo command if something actually changed
    if(dialog->exec() == QDialog::Accepted)
        pushUndo(new EditTransitionCommand(*this,transition,condition_text_before,transit_text_before,dialog->conditionText(),dialog->transitText()));
}


void Model::linkNode(Node* node)
{
    // and show in scene
    node->setVisible(true);
}


void Model::unlinkNode(Node* node)
{
    // and remove from scene
    node->setVisible(false);
}


void Model::linkTransition(Transition* transition)
{
    // and show in scene
    transition->setVisible(true);
}


void Model::unlinkTransition(Transition* transition)
{
    // and remove from scene
    transition->setVisible(false);
}
