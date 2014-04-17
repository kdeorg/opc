#ifndef TRIADA_H
#define TRIADA_H
#include <QString>
#include <QList>
class Node;
class Triada
{
public:
    int index;
    QString name;
    QString arg1;
    QString arg2;
    Triada(QString name);
    Triada(Node *name);
    QString toString();
    static Triada *label(QString name);
    static Triada *ifZeroGoTo();
    static Triada *goTo(QString name);
    void setArg(int index,int num);
    void setArg(int index,Node *node);
    void setArg(int index,Triada *triada);
    void addToList(QList<Triada*> *list);
};

#endif // TRIADA_H
