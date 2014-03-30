#include "rule.h"

Rule::Rule(int id_, QString* left, QList<QString>* right):
    right(right), left(*left), id(id_)
{

}

Rule::~Rule(){
    delete right;
}

QString Rule::getLeft(){
    return left;
}

QList<QString>* Rule::getRight(){
    return right;
}

void Rule::print(){
    qDebug() <<left <<" -> ";
    QListIterator<QString> i (*right);
    while(i.hasNext()){
       qDebug() << i.next() <<" ";
    }
    qDebug() <<endl;
}

int Rule::getId(){
    return id;
}
