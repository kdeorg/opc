#ifndef DEFINES_H
#define DEFINES_H

typedef struct{
    int id;
    QString type;
    //QString name;
}TokenType;

typedef struct{
    int id;
    QString tokclass;
    QString toktype;
    QString value;
}Token;

#endif // DEFINES_H
