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
                // We'll add it to the map.
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
