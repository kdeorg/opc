#ifndef VARIABLE_H
#define VARIABLE_H

#include "includes.h"

class Variable
{
public:
    QString name;
    Type *type;
    bool constant;
    QString value;

    Variable(Node *node);
    void setType(Type *t);
    void setIsConst(bool arg);
    void setValue(int value);
    void setValue(QString value);
};

class Vars:public QList<Variable *>
{
public:
    Vars();
    void print(QTextEdit *txtTriads);
    Variable *findVarByName(QString name);
    void append(Variable *arg);
};

#endif // VARIABLE_H
