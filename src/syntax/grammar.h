#ifndef GRAMMAR_H
#define GRAMMAR_H
#include <QList>
#include <QString>
#include <QMap>
#include "rule.h"
#include <QDebug>


class Grammar
{
private:
    QList<QString>* terminals;
    QList<QString>* nonterminals;
    QString start;
    QMap<int,Rule*>* rules;
    QString name;

    QMap<QString, QList<QString>*> *FIRST;

    QMap<QPair<QString, QString>, QPair<QList<QString>*, int>* > *table;
public:
    Grammar();
    ~Grammar();
    int checkGrammar();
    int addTerminal(QString* terminal);
    int addNonterminal(QString* nonterminal);
    int setStart(QString* start);
    void setName(QString* name);
    int addRule(int num, QList<QString>* right, QString* left);

    QString getStart();
    QString getName();
    Rule* getRule(int number);
    QMap<QString, QList<QString>*> * getFIRST();

    void print();
    void generateFIRST_SET();
    void generateFIRST_SET_forNonterminal(QString* nonterminal, QList<QString>* result);

    void generateFIRST_SET_with_eps();
    void generateFIRST_SET_forNonterminal_with_eps(QString* nonterminal, QList<QString>* result);

    int isTerminal(QString* lex);
    int isNonterminal(QString* lex);

    void generateTable();
    QMap<QPair<QString, QString>, QPair<QList<QString>*, int>* > * getTable();
};

#endif // GRAMMAR_H
