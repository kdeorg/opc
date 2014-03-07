#include "lex_analyzer.h"

Lex_analyzer::Lex_analyzer(QMap<QString, TokenType*> *keywords,
                           QMap<QString, TokenType*> *types,
                           QMap<QString, TokenType*> *operators,
                           QMap<QString, TokenType*> *ids,
                           QMap<QString, TokenType*> *constants):
    keywords(keywords),types(types),operators(operators),ids(ids),constants(constants)
{
    tokens = new QList<Token*>();
    if(keywords == NULL || types == NULL || operators == NULL || ids == NULL || constants == NULL){
        qDebug() <<"Error empty input language tables for lex analyzer!" <<endl;
    }
}

QList<Token*>* Lex_analyzer::analyze(QString filename){
    QFile* file = new QFile(filename);
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text)) {
        printf("Couldn't open file\n");
        return NULL;
    }
    QTextStream in(file);
    int i=0;
    QChar c;

    char flag_separator_started = 0;
    QChar two_liter_separator_buf;

    char in_progress = 6;
    QString buffer = "";

    while (!in.atEnd()){
        in >> c;
        qDebug() <<"char[" <<i <<"] = " <<c <<endl;
        if(flag_separator_started){
            if(is_end_sep(c)){
                if(checkTwoCharSeparator(two_liter_separator_buf,c)){
                    buffer.append(c);
                    qDebug() <<"char[" <<i <<"] = " <<c <<"is end sep"
                            <<"separator = " <<buffer <<endl;
                    flag_separator_started = 0;
                    continue;
                }
            }
            flag_separator_started = 0;
        }
        switch(getLiterType(c)){
        case 1:
            qDebug() <<"char[" <<i <<"] = " <<c <<"is litter" <<endl;
            if(in_progress != 1)
                 if(in_progress != 6){
                    addToken(in_progress, buffer);
                    buffer = "";
                 }
            in_progress = 1;
            buffer.append(c);
            break;
        case 2:
            qDebug() <<"char[" <<i <<"] = " <<c <<"is digit" <<endl;
            if(in_progress != 2)
                 if(in_progress != 6){
                    addToken(in_progress, buffer);
                    buffer = "";
                 }
            in_progress = 2;
            buffer.append(c);
            break;
        case 3:
            qDebug() <<"char[" <<i <<"] = " <<c <<"is one lit sep" <<endl;
            if(in_progress != 6){
                addToken(in_progress, buffer);
                buffer = "";
            }
            in_progress = 3;
            buffer.append(c);
            break;
        case 4:
            qDebug() <<"char[" <<i <<"] = " <<c <<"is begin sep" <<endl;
            flag_separator_started = 1;
            two_liter_separator_buf = c;
            if(in_progress != 6){
                addToken(in_progress, buffer);
                buffer = "";
            }
            in_progress = 4;
            buffer.append(c);
            break;
        /*case 5:
            qDebug() <<"char[" <<i <<"] = " <<c <<"is end sep" <<endl;
            break;*/
        case 6:
            qDebug() <<"char[" <<i <<"] = " <<c <<"is missed" <<endl;
            if(in_progress != 6){
                addToken(in_progress, buffer);
                buffer = "";
            }
            in_progress = 6;
            break;
        case 0:
        default:
            qDebug() <<"char[" <<i <<"] = " <<c <<"is undefined" <<endl;
            exit(1);
        }


        i++;
    }
    //if file finishes with literal - not space \t or \n
    if(in_progress != 6){
        addToken(in_progress, buffer);
        buffer = "";
    }
    file->close();
    return this->tokens;
}

int Lex_analyzer::getLiterType(QChar c){
    if(is_letter(c))
        return 1;
    else if(is_digit(c))
        return 2;
    else if(is_one_lit_sep(c))
        return 3;
    else if(is_begin_sep(c))
        return 4;
    else if(is_end_sep(c))
        return 5;
    else if(is_missed_sym(c))
        return 6;
    else return 0;
}

int Lex_analyzer::is_letter(QChar c){
    if((c.toAscii() > 64 && c.toAscii() < 91)
            ||(c.toAscii() > 96 && c.toAscii() < 123))
        return 1;
    else
        return 0;
}

int Lex_analyzer::is_digit(QChar c){
    if(c.toAscii() > 47 && c.toAscii() < 58)
        return 1;
    else
        return 0;
}

int Lex_analyzer::is_one_lit_sep(QChar c){
    if(c == '+' || c == '-' || c == '*' || c == '/'||
       c == ',' || c == ';' || c == '=' ||
       c == ']' || c == '[' || c == '{' || c == '}'||
       c == '(' || c == ')' )
        return 1;
    else
        return 0;
}

int Lex_analyzer::is_begin_sep(QChar c){
    if(c == '>' || c == '<' || c == '.' || c == ':' )
        return 1;
    else
        return 0;
}

int Lex_analyzer::is_end_sep(QChar c){
    if(c == '>' || c == '=' || c == '.' )
        return 1;
    else
        return 0;
}

int Lex_analyzer::is_missed_sym(QChar c){
    if(c == ' '|| c=='\t' || c=='\n')
        return 1;
    else
        return 0;
}

int Lex_analyzer::checkTwoCharSeparator(QChar part1, QChar part2){
    if(part1 == '<'){
        if(part2 == '>'||part2 == '=')
            return 1;
        else
            return 0;
    }else if(part1 == '>'){
        if(part2 == '=')
            return 1;
        else
            return 0;
    }else if(part1 =='.'){
        if(part2 == '.')
            return 1;
        else
            return 0;
    }else if(part1 ==':'){
        if(part2 == '=')
            return 1;
        else
            return 0;
    }else
        return 0;
}

int Lex_analyzer::is_keyword(QString tokenValue, TokenType* resultTokenInTable){
    TokenType *t;
    if(!keywords->contains(tokenValue))
        return 0;
    else{
        t = keywords->value(tokenValue);
        resultTokenInTable->id = t->id;
        resultTokenInTable->type = t->type;
    }
    return 1;
}

int Lex_analyzer::is_type(QString tokenValue, TokenType* resultTokenInTable){
    TokenType *t;
    if(!types->contains(tokenValue))
        return 0;
    else{
        t = types->value(tokenValue);
        resultTokenInTable->id = t->id;
        resultTokenInTable->type = t->type;
    }
    return 1;
}

int Lex_analyzer::is_id(QString tokenValue, TokenType* resultTokenInTable){
    TokenType *t;
    if(ids->contains(tokenValue)){
        t = ids->value(tokenValue);
        resultTokenInTable->id = t->id;
        resultTokenInTable->type = t->type;
    }else{
        //look for element with max id in table
        int id = 0;
        QMap<QString, TokenType*>::iterator i;
        for(i = ids->begin(); i!=ids->end(); i++){
            if(i.value()->id > id)
                id = i.value()->id;
        }
        t = new TokenType;
        t->id = id+1;
        t->type = "ID";
        ids->insert(tokenValue,t);
        resultTokenInTable->id = t->id;
        resultTokenInTable->type = t->type;
    }
    return 1;
}

int Lex_analyzer::is_const(QString tokenValue, TokenType* resultTokenInTable){
    TokenType *t;
    if(constants->contains(tokenValue)){
        t = constants->value(tokenValue);
        resultTokenInTable->id = t->id;
        resultTokenInTable->type = t->type;
    }else{
        //look for element with max id in table
        int id = 0;
        QMap<QString, TokenType*>::iterator i;
        for(i = constants->begin(); i!=constants->end(); i++){
            if(i.value()->id > id)
                id = i.value()->id;
        }
        t = new TokenType;
        t->id = id+1;
        t->type = "CINT";
        constants->insert(tokenValue,t);
        resultTokenInTable->id = t->id;
        resultTokenInTable->type = t->type;
    }
    return 1;
}

int Lex_analyzer::is_operator(QString tokenValue, TokenType* resultTokenInTable){
    TokenType *t;
    if(!operators->contains(tokenValue))
        return 0;
    else{
        t = operators->value(tokenValue);
        resultTokenInTable->id = t->id;
        resultTokenInTable->type = t->type;
    }
    return 1;
}

void Lex_analyzer::addToken(int tokenType, QString tokenValue){
    TokenType *t = new TokenType;
    Token *tok = new Token;
    switch(tokenType){
    case 1:
        if(is_keyword(tokenValue,t)){
            qDebug() <<"Token" <<tokenValue <<"found in keywords: id=" <<t->id  <<"type=" <<t->type <<endl;
            tok->id=t->id;
            tok->tokclass = "keywords";
            tok->toktype = t->type;
            tok->value = tokenValue;
            tokens->append(tok);
        }else if(is_type(tokenValue,t)){
            qDebug() <<"Token" <<tokenValue <<"found in types: id=" <<t->id  <<"type=" <<t->type <<endl;
            tok->id=t->id;
            tok->tokclass = "types";
            tok->toktype = t->type;
            tok->value = tokenValue;
            tokens->append(tok);
        }else{
            is_id(tokenValue, t);
            qDebug() <<"Token" <<tokenValue <<"found in/inserted to ids: id=" <<t->id  <<"type=" <<t->type <<endl;
            tok->id=t->id;
            tok->tokclass = "ids";
            tok->toktype = t->type;
            tok->value = tokenValue;
            tokens->append(tok);
        }
        break;
    case 2:
        is_const(tokenValue, t);
        qDebug() <<"Token" <<tokenValue <<"found in/inserted to constants: id=" <<t->id  <<"type=" <<t->type <<endl;
        tok->id=t->id;
        tok->tokclass = "constants";
        tok->toktype = t->type;
        tok->value = tokenValue;
        tokens->append(tok);
        break;
    case 3:
    case 4:
        if(is_operator(tokenValue, t)){
            qDebug() <<"Token" <<tokenValue <<"found in operators/separators: id=" <<t->id  <<"type=" <<t->type <<endl;
            tok->id=t->id;
            tok->tokclass = "separators";
            tok->toktype = t->type;
            tok->value = tokenValue;
            tokens->append(tok);
        }else{
            qDebug() <<"Error unknown token type: " <<tokenType  <<endl;
        }
        break;
    default:
        qDebug() <<"Error unknown token type: " <<tokenType  <<endl;
    }
    qDebug() <<"Adding token of type[" <<tokenType <<"] = " <<tokenValue  <<endl;
}
