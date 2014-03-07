#include <QCoreApplication>
#include "xmlreader.h"
#include "lex_analyzer.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    std::cout<< "Hello\n";
    QMap<QString, TokenType*> *keywords = new QMap<QString, TokenType*>;
    QMap<QString, TokenType*> *types = new QMap<QString, TokenType*>;
    QMap<QString, TokenType*> *operators = new QMap<QString, TokenType*>;
    QMap<QString, TokenType*> *ids = new QMap<QString, TokenType*>;
    QMap<QString, TokenType*> *constants = new QMap<QString, TokenType*>;
    XMLReader *XMLR = new XMLReader(keywords, types, operators, ids, constants);
    XMLR->parse_XML("SRCTables.xml");
    Lex_analyzer * lex = new Lex_analyzer(keywords, types, operators, ids, constants);
    XMLR->writeTokensXML("Tokens-res.xml", lex->analyze(argv[1]));
    XMLR->writeTablesXML("SRCTables-res.xml");
    delete (keywords);
    delete (types);
    delete (operators);
    delete (ids);
    delete (constants);
    delete (lex);
    std::cout<< "\n finished\n\n";
    return 0;
}
