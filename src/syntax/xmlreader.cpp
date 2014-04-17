#include "xmlreader.h"

XMLReader::XMLReader(QMap<QString, TokenType*> *keywords,
                     QMap<QString, TokenType*> *types,
                     QMap<QString, TokenType*> *operators,
                     QMap<QString, TokenType*> *ids,
                     QMap<QString, TokenType*> *constants):
    keywords(keywords),types(types),operators(operators),ids(ids),constants(constants)
{

}

void XMLReader::parse_XML(QString filename){
    QFile* file = new QFile(filename);
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text)) {
        printf("Couldn't open file\n");
        return;
    }
    QXmlStreamReader xmlFile(file);
    while(!xmlFile.atEnd() && !xmlFile.hasError()) {
            // Read next element.
            QXmlStreamReader::TokenType token = xmlFile.readNext();
            // If token is just StartDocument, we'll go to next.
            if(token == QXmlStreamReader::StartDocument) {
                continue;
            }
            // If token is StartElement, we'll see if we can read it.
            if(token == QXmlStreamReader::StartElement) {
                this->parseTokens(&xmlFile);
            }
        }
    file->close();
}

void XMLReader::parseTokens(QXmlStreamReader *xmlFile){
    if(xmlFile->name() == "KEYWORDS"||xmlFile->name() == "keywords" ) {
        QString elementName = xmlFile->text().toString();
        qDebug() <<elementName <<endl;
        this->parseTokenGroup(xmlFile, "keyword", "KEYWORD", this->keywords);
    }else if(xmlFile->name() == "DATA_TYPES"||xmlFile->name() == "data_types"){
        this->parseTokenGroup(xmlFile, "data_type", "DATA_TYPE", this->types);
    }else if(xmlFile->name() == "OPERATORS"||xmlFile->name() == "operators"){
        this->parseTokenGroup(xmlFile, "operator", "OPERATOR", this->operators);
    }
}

void XMLReader::parseTokenGroup(QXmlStreamReader *xmlFile,
                              QString tokName_loc,
                              QString tokName_upc,
                              QMap<QString, TokenType*> *base){
    QString name;
    QString id;
    QString type;
    char full_keyword = 0;
    char keyword_started = 0;
    QString tokGrName_loc = tokName_loc;
    QString tokGrName_upc = tokName_upc;
    tokGrName_loc.append("s");
    tokGrName_upc.append("S");
    QXmlStreamReader::TokenType token = xmlFile->readNext();
    while(  (token != QXmlStreamReader::EndElement ||xmlFile->name() != tokGrName_upc)
          &&(token != QXmlStreamReader::EndElement || xmlFile->name() != tokGrName_loc)
          &&(!xmlFile->atEnd() && !xmlFile->hasError())){
        if(keyword_started ==1){
            if(xmlFile->name()=="type" || xmlFile->name()=="TYPE"){
                full_keyword |= 1;
                QString elementName = xmlFile->readElementText();
                type = elementName;
                qDebug() <<elementName <<endl;
            }else
            if(xmlFile->name()=="name" || xmlFile->name()=="NAME"){
                full_keyword |= 2;
                QString elementName = xmlFile->readElementText();
                name = elementName;
                qDebug() <<elementName <<endl;
            }else{
                //qDebug() <<"Unrecognized token in keyword id=" <<id <<" section" <<endl;
            }
        }
        if(xmlFile->name()== tokName_loc ||xmlFile->name()==tokName_upc){
            if(token == QXmlStreamReader::StartElement){
            QXmlStreamAttributes attributes = xmlFile->attributes();
            if(attributes.hasAttribute("id"))
            {
                //add it to the map.
                QString attr_id = attributes.value("id").toString();
                id = attr_id;
                qDebug()<<"id:"<<attr_id<<endl;
                keyword_started = 1;
            }
            }else if(token == QXmlStreamReader::EndElement){
            if(full_keyword == 3){
                qDebug()<<"Adding" <<tokName_loc <<" id=" <<id <<"to base"<<endl;
                TokenType* t = new TokenType();
                t->id = id.toInt();
                t->type = type;
                base->insert(name,t);
                full_keyword = 0;
                keyword_started = 0;
            }else
                qDebug()<<tokName_loc <<" id=" <<id <<"is corrupted"<<endl;
            }
        }
        token = xmlFile->readNext();
    }
    qDebug() <<"parse " <<tokName_loc <<" finished" <<endl;
}

void XMLReader::writeTablesXML(QString filename){

    QFile* file = new QFile(filename);
    if (!file->open(QIODevice::WriteOnly | QIODevice::Text)) {
        printf("Couldn't open file\n");
        return;
    }
    QXmlStreamWriter* xmlWriter = new QXmlStreamWriter();
    xmlWriter->setDevice(file);
    xmlWriter->setAutoFormatting(true);
    xmlWriter->writeStartDocument();
    if(keywords != NULL)
        this->writeTokenGroup(xmlWriter, "keyword", keywords);
    if(types != NULL)
        this->writeTokenGroup(xmlWriter, "data_type", types);
    if(operators != NULL)
        this->writeTokenGroup(xmlWriter, "operator", operators);
    if(ids != NULL)
        this->writeTokenGroup(xmlWriter, "id", ids);
    if(constants != NULL)
        this->writeTokenGroup(xmlWriter, "constant", constants);
    xmlWriter->writeEndDocument();
    delete xmlWriter;
    file->close();
}

void XMLReader::writeTokenGroup(QXmlStreamWriter *xmlFile, QString tokName_loc, QMap<QString, TokenType*> *base){
    QString tokGrName_loc = tokName_loc;
    tokGrName_loc.append("s");
    xmlFile->writeStartElement(tokGrName_loc);
    qDebug() <<"Base size = " <<base->size() <<endl;
    QMap<QString, TokenType*>::iterator i;
    for(i = base->begin(); i!=base->end(); i++){
        xmlFile->writeStartElement(tokName_loc);
        xmlFile->writeAttribute("id", QString::number(i.value()->id));
        xmlFile->writeTextElement("type",i.value()->type);
        xmlFile->writeTextElement("name",i.key());
        xmlFile->writeEndElement();
    }
    xmlFile->writeEndElement();
}

void XMLReader::writeTokensXML(QString filename, QList<Token*> *tokens){
    QFile* file = new QFile(filename);
    if (!file->open(QIODevice::WriteOnly | QIODevice::Text)) {
        printf("Couldn't open file\n");
        return;
    }
    QXmlStreamWriter* xmlWriter = new QXmlStreamWriter();
    xmlWriter->setDevice(file);
    xmlWriter->setAutoFormatting(true);
    xmlWriter->writeStartDocument();
    xmlWriter->writeStartElement("tokens");
    QList<Token*>::iterator i;
    int j = 0;
    for(i = tokens->begin(); i!=tokens->end(); i++){
        xmlWriter->writeStartElement("token");
        xmlWriter->writeAttribute("id", QString::number(j));
        xmlWriter->writeAttribute("class", tokens->at(j)->tokclass);
        QString tid = "";
        tid.append(tokens->at(j)->toktype);
        tid.append(QString::number(tokens->at(j)->id));
        xmlWriter->writeTextElement("type",tid);
        xmlWriter->writeTextElement("value",tokens->at(j)->value);
        xmlWriter->writeEndElement();
        j++;
    }
    xmlWriter->writeEndElement();
    xmlWriter->writeEndDocument();
    delete xmlWriter;
    file->close();
}

void XMLReader::setGrammars(QList<Grammar*> *grammars){
    this->grammars = grammars;
}

int XMLReader::readGrammars(QString *filename){
    if(grammars == NULL){
        qDebug() << "Error! No grammars buffer set in xmlreader.\n";
        return 0;
    }else{
        QFile* file = new QFile(*filename);
        if (!file->open(QIODevice::ReadOnly | QIODevice::Text)) {
            printf("Error! Couldn't open file\n");
            return 0;
        }
        QXmlStreamReader xmlFile(file);
        char grammarsStartedFlag = 0;
        while(!xmlFile.atEnd() && !xmlFile.hasError()) {
                // Read next element.
                QXmlStreamReader::TokenType token = xmlFile.readNext();
                // If token is just StartDocument, we'll go to next.
                if(token == QXmlStreamReader::StartDocument) {
                    continue;
                }
                // If token is StartElement, we'll see if we can read it.
                if(token == QXmlStreamReader::StartElement) {
                    if(xmlFile.name() == "GRAMMARS"||xmlFile.name() == "grammars" ){
                        grammarsStartedFlag = 1;
                        continue;
                    }else if(xmlFile.name() == "GRAMMAR"||xmlFile.name() == "grammar" ){
                        Grammar* newGrammar = new Grammar();

                        QXmlStreamAttributes attributes = xmlFile.attributes();
                        if(attributes.hasAttribute("name"))
                        {
                            QString name = attributes.value("name").toString();
                            newGrammar->setName(&name);
                        }

                        if(readGrammar(&xmlFile, newGrammar)){
                            grammars->append(newGrammar);
                            qDebug() <<"New grammar: " <<newGrammar->getName() <<" succesfully appended" <<endl;
//                            getwchar();
                        }else{
                            qDebug() <<"Error reading grammar " <<newGrammar->getName() <<endl;
//                            getwchar();
                        }
                    }
                }
            }
        file->close();
    }
}

int XMLReader::readGrammar(QXmlStreamReader *xmlFile, Grammar* grammar){
    char flag_grammarIsFull = 0;
    QXmlStreamReader::TokenType token = xmlFile->readNext();
    while(  (token != QXmlStreamReader::EndElement ||xmlFile->name() != "GRAMMAR")
            &&(token != QXmlStreamReader::EndElement || xmlFile->name() != "grammar")
          &&(!xmlFile->atEnd() && !xmlFile->hasError())){
        if(token == QXmlStreamReader::StartElement) {
            if(xmlFile->name() == "NON_TERMINALS"||xmlFile->name() == "non_terminals" ){
                flag_grammarIsFull |= 0x1;
                qDebug() <<"Nonterminals:" <<endl;
                if(this->readNonterminals(xmlFile, grammar) == 0)
                    return 0;
            }else if(xmlFile->name() == "TERMINALS"||xmlFile->name() == "terminals" ){
                flag_grammarIsFull |= 0x2;
                qDebug() <<"Terminals:" <<endl;
                if(this->readTerminals(xmlFile, grammar) == 0)
                    return 0;
            }else if(xmlFile->name() == "RULES"||xmlFile->name() == "rules" ){
                qDebug() <<"Rules:" <<endl;
                if(this->readRules(xmlFile, grammar) == 0)
                    return 0;
                flag_grammarIsFull |= 0x4;
            }else if(xmlFile->name() == "START"||xmlFile->name() == "start" ){
                qDebug() <<"Start:" <<endl;
                QString readstart = xmlFile->readElementText();
                grammar->setStart(&readstart);
                flag_grammarIsFull |= 0x8;
            }
        }
        token = xmlFile->readNext();
    }
    if(flag_grammarIsFull == 0xF)
        return 1;
    else
        return 0;
}

int XMLReader::readTerminals(QXmlStreamReader *xmlFile, Grammar* grammar){
    QXmlStreamReader::TokenType token = xmlFile->readNext();
    while(  (token != QXmlStreamReader::EndElement ||xmlFile->name() != "TERMINALS")
            &&(token != QXmlStreamReader::EndElement || xmlFile->name() != "terminals")
          &&(!xmlFile->atEnd() && !xmlFile->hasError())){
        if(token == QXmlStreamReader::StartElement) {
            if(xmlFile->name()=="TERMINAL" || xmlFile->name()=="terminal"){
                QString readterminal = xmlFile->readElementText();
                grammar->addTerminal(&readterminal);
                qDebug() <<readterminal <<endl;
            }else{
                qDebug() <<"Error! Wrong token in terminals section in " <<grammar->getName() <<" grammar." <<endl;
                return 0;
            }
        }
        token = xmlFile->readNext();
    }
    return 1;
}

int XMLReader::readNonterminals(QXmlStreamReader *xmlFile, Grammar* grammar){
    QXmlStreamReader::TokenType token = xmlFile->readNext();
    while(  (token != QXmlStreamReader::EndElement ||xmlFile->name() != "NON_TERMINALS")
            &&(token != QXmlStreamReader::EndElement || xmlFile->name() != "non_terminals")
          &&(!xmlFile->atEnd() && !xmlFile->hasError())){
        if(token == QXmlStreamReader::StartElement) {
            if(xmlFile->name()=="NON_TERMINAL" || xmlFile->name()=="non_terminal"){
                QString readnnonterminal = xmlFile->readElementText();
                grammar->addNonterminal(&readnnonterminal);
                qDebug() <<readnnonterminal <<endl;
            }else{
                qDebug() <<"Error! Wrong token in non_terminals section in " <<grammar->getName() <<" grammar." <<endl;
                return 0;
            }
        }
        token = xmlFile->readNext();
    }
    return 1;
}

int XMLReader::readRules(QXmlStreamReader *xmlFile, Grammar* grammar){
    QXmlStreamReader::TokenType token = xmlFile->readNext();
    int last_rule_id = 0;
    while(  (token != QXmlStreamReader::EndElement ||xmlFile->name() != "RULES")
            &&(token != QXmlStreamReader::EndElement || xmlFile->name() != "rules")
          &&(!xmlFile->atEnd() && !xmlFile->hasError())){
        if(token == QXmlStreamReader::StartElement) {
            if(xmlFile->name()=="RULE" || xmlFile->name()=="rule"){
                //QString readterminal = xmlFile->readElementText();
                //grammar->addTerminal(&readterminal);
                //qDebug() <<readterminal <<endl;
                QXmlStreamAttributes attributes = xmlFile->attributes();
                if(attributes.hasAttribute("id"))
                {
                    QString id = attributes.value("id").toString();
                    int id_int = id.toInt();
                    if(id_int != last_rule_id + 1){
                        qDebug() <<"Error! Wrong token in rules section in " <<grammar->getName() <<" grammar."
                                <<"Irreqular rule id numbering in "<<last_rule_id+1 <<" id." <<endl;
                        return 0;
                    }else
                        this->readRule(xmlFile, grammar, id_int);
                    last_rule_id++;

                }else{
                    qDebug() <<"Error! Wrong token in rules section in " <<grammar->getName() <<" grammar."
                            <<"No rule id." <<endl;
                    return 0;
                }
            }else{
                qDebug() <<"Error! Wrong token in rules section in " <<grammar->getName() <<" grammar." <<endl;
                return 0;
            }
        }
        token = xmlFile->readNext();
    }
    return 1;
}

int XMLReader::readRule(QXmlStreamReader *xmlFile, Grammar* grammar, int id_int){
    QXmlStreamReader::TokenType token = xmlFile->readNext();
    int i = 0;
    QString left;
    char hasLeft = 0;
    QList<QString> *right = new QList<QString>();
    while(  (token != QXmlStreamReader::EndElement ||xmlFile->name() != "RULE")
            &&(token != QXmlStreamReader::EndElement || xmlFile->name() != "rule")
          &&(!xmlFile->atEnd() && !xmlFile->hasError())){
        if(token == QXmlStreamReader::StartElement) {
            if(xmlFile->name()=="LEFT" || xmlFile->name()=="left"){
                left = xmlFile->readElementText();
                //grammar->addTerminal(&readterminal);
                qDebug() <<left <<endl;
                hasLeft ++;
            }else if(xmlFile->name()=="RIGHT" || xmlFile->name()=="right"){
                QString readright = xmlFile->readElementText();
                right->append(readright);
                qDebug() <<readright <<endl;
            }else{
                qDebug() <<"Error! Wrong token in rules() section(" <<id_int <<")" <<i+1 <<") section in "
                        <<grammar->getName() <<" grammar." <<endl;
                return 0;
            }
            i++;
        }
        token = xmlFile->readNext();
    }
    if(hasLeft && i>1){
        //qDebug() <<"Adding new rule" <<endl;
        grammar->addRule(id_int, right, &left);
        //getwchar();
    }else{
        qDebug() <<"Error! Wrong token in rules(" <<i+1 <<") section in " <<grammar->getName() <<" grammar." <<endl;
    }
    return 1;
}
