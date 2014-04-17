#ifndef TYPE_H
#define TYPE_H
#include "includes.h"
class Node;
class Type
{
public:
    QString name;
    QString baseType;
    Type(QString name);
    void setBaseType(Node *node);
    void setBaseType(QString str);
    virtual QString toString();
    virtual bool isSimple();
    virtual bool isRange();
    virtual bool isEnum();
    virtual bool isArray();
};

class SimpleType:public Type
{
public:
    SimpleType(QString name);
    virtual QString toString();
    virtual bool isSimple();
};

class RangeType:public Type
{
    QString min,max;
public:
    RangeType(QString name);
    void setDiapazon(Node *min, Node *max);
    virtual QString toString();
    virtual bool isRange();
};

class EnumType:public Type
{
public:
    static int counter;
    EnumType(QString name);
    virtual QString toString();
    virtual bool isEnum();
};

class ArrayType:public Type
{
    QList<QString> *firsts;
    QList<QString> *seconds;
public:
    ArrayType(QString name);
    void addDimension(Node *min, Node *max);
    virtual QString toString();
    virtual bool isArray();
};

class Types:public QList<Type *>
{
    static int nameGenerator;
public:
    Types();
    void print(QTextEdit *txtTriads);
    SimpleType *getInteger();
    SimpleType *getRational();
    QString getFreeName();
    Type *findTypeByName(QString name);
    void append(Type *arg);
};

#endif // TYPE_H
