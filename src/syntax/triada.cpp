#include "triada.h"
#include "node.h"
Triada::Triada(QString name)
{
    this->name = name;
}

Triada::Triada(Node *name)
{
    this->name = name->value;
}

QString Triada::toString()
{
    return QString::number(index)+")    ["+name+",    "+arg1+",    "+arg2+"]";
}

Triada *Triada::label(QString name)
{
    return new Triada(">label");//|"+name);
}
Triada *Triada::ifZeroGoTo()
{
    return new Triada(">ifZeroGoTo");
}
Triada *Triada::goTo(QString name)
{
    return new Triada(">goTo");//|"+name);
}

void Triada::setArg(int index, int num)
{
    if(index==0){
        arg1 = ""+QString::number(num);
    }else{
        arg2 = ""+QString::number(num);
    }
}

void Triada::setArg(int index, Node *node)
{
    if(index==0){
        arg1 = ""+node->value;
    }else{
        arg2 = ""+node->value;
    }
}

void Triada::setArg(int index, Triada *triada)
{
    if(index==0){
        arg1 = "t "+QString::number(triada->index);
    }else{
        arg2 = "t "+QString::number(triada->index);
    }
}

void Triada::addToList(QList<Triada *> *list)
{
    index = Node::triIndexGenerator++;
    list->append(this);
}
