#ifndef XMLREADER_H
#define XMLREADER_H
#include <QFile>
#include <QXmlStreamReader>
#include <QMap>
#include <qdebug.h>
#include <iostream>

#include "defines.h"
#include "grammar.h"
#include "stdlib.h"
#include "stdio.h"

class XMLReader
{
private:
    //QMap<QString,QList <QMap <QString, Token> > > tokenBase;
    QMap <QString, TokenType*> *keywords;
    QMap <QString, TokenType*> *operators;
    QMap <QString, TokenType*> *types;
    QMap <QString, TokenType*> *ids;
    QMap <QString, TokenType*> *constants;

    QList<Grammar*> *grammars;
public:
    XMLReader(QMap<QString, TokenType*> *keywords,
              QMap<QString, TokenType*> *types,
              QMap<QString, TokenType*> *operators,
              QMap<QString, TokenType*> *ids,
              QMap<QString, TokenType*> *constants);
    void parse_XML(QString filename);
    void parseTokens(QXmlStreamReader *xmlFile);
    void parseTokenGroup(QXmlStreamReader *xmlFile, QString tokName_loc, QString tokName_upc, QMap<QString, TokenType*> *base);

    void writeTablesXML(QString filename);
    void writeTokenGroup(QXmlStreamWriter *xmlFile, QString tokName_loc, QMap<QString, TokenType*> *base);

    void writeTokensXML(QString filename, QList<Token*> *tokens);

    void setGrammars(QList<Grammar*> *grammars);
    int readGrammars(QString *filename);
    int readGrammar(QXmlStreamReader *xmlFile, Grammar* grammar);

    int readTerminals(QXmlStreamReader *xmlFile, Grammar* grammar);
    int readNonterminals(QXmlStreamReader *xmlFile, Grammar* grammar);
    int readRules(QXmlStreamReader *xmlFile, Grammar* grammar);

    int readRule(QXmlStreamReader *xmlFile, Grammar* grammar, int id_int);
};

#endif // XMLREADER_H
