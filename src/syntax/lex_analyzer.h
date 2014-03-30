#ifndef LEX_ANALYZER_H
#define LEX_ANALYZER_H

#include <QString>
#include <QFile>
#include <QDebug>
#include <stdlib.h>
#include <stdio.h>
#include <QMap>

#include "defines.h"

class Lex_analyzer
{
private:
    QList<Token*> *tokens;

    //tables:
    QMap <QString, TokenType*> *keywords;
    QMap <QString, TokenType*> *operators;
    QMap <QString, TokenType*> *types;
    QMap <QString, TokenType*> *ids;
    QMap <QString, TokenType*> *constants;

    int getLiterType(QChar c);

    int is_letter(QChar c);
    int is_digit(QChar c);
    int is_one_lit_sep(QChar c);
    int is_begin_sep(QChar c);
    int is_end_sep(QChar c);
    int is_missed_sym(QChar c);

    int checkTwoCharSeparator(QChar part1, QChar part2);

    void addToken(int tokenType, QString tokenValue);

    int is_keyword(QString tokenValue, TokenType* resultTokenInTable);
    int is_type(QString tokenValue, TokenType* resultTokenInTable);
    int is_id(QString tokenValue, TokenType* resultTokenInTable);
    int is_const(QString tokenValue, TokenType* resultTokenInTable);
    int is_operator(QString tokenValue, TokenType* resultTokenInTable);

public:
    Lex_analyzer(QMap<QString, TokenType*> *keywords,
                 QMap<QString, TokenType*> *types,
                 QMap<QString, TokenType*> *operators,
                 QMap<QString, TokenType*> *ids,
                 QMap<QString, TokenType*> *constants);

    QList<Token*>* analyze(QString filename);
    //QList<Token*>* getTokens();
    QList<Token*> * getTokens();
};

#endif // LEX_ANALYZER_H
