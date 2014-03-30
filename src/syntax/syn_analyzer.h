#ifndef SYN_ANALYZER_H
#define SYN_ANALYZER_H

#include <QStack>
#include"grammar.h"
#include "defines.h"
#include <QtGui>


class Syn_analyzer
{
private:
    QStack<Token*> *tokens;
    QStack<QString> *buffer;
    QList<QPair<QString, int>* >* rule_order;

    QList<Grammar*> *grammars;
    QStandardItem *rootNode;

public:
    Syn_analyzer(QList<Token*> *tokens_, QList<Grammar*> *gr, QStandardItem *rootNode);
    ~Syn_analyzer();

    int analyze();

    void setGrammars(QList<Grammar*> *gr);
    Grammar* getGrammar(QString gr);
};

#endif // SYN_ANALYZER_H
