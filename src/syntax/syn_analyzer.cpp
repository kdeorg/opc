#include "syn_analyzer.h"

Syn_analyzer::Syn_analyzer(QList<Token*> *tokens_, QList<Grammar*> *gr, QStandardItem *rootNode):
    grammars(gr), rootNode(rootNode)
{
    tokens = new QStack<Token*>();
    buffer = new QStack<QString>();
    rule_order = new QList<QPair<QString, int>* >();

    QListIterator<Token*> l (*tokens_);

    while(l.hasNext()){
        Token* t = l.next();
        if(t->tokclass!="keywords" && t->tokclass!="ids" && t->tokclass!="constants"){
            QString id = QString::number(t->id);
            t->toktype.append(id);
        }
        tokens->push_front(t);
        qDebug() << t->toktype;
    }
    buffer->push("finish");
    buffer->push("S");
}

Syn_analyzer::~Syn_analyzer(){

}

Grammar* Syn_analyzer::getGrammar(QString gr){
    Grammar* current;
    QListIterator<Grammar*> i(*grammars);
    while(i.hasNext()){
        current = i.next();
        if(current->getName() == gr)
            return current;
    }
    return NULL;
}

int isGrammar(QString lex){
    if(lex.at(0)=='G')
        if(lex.at(1)=='R')
            if(lex.at(2)=='A')
                if(lex.at(3)=='M')
                    return 1;
    return 0;
}

void copyStacks(QStack<Token*> * t, QStack<Token*> * t_hold, QStack<QString>* b, QStack<QString> *b_hold ){
    QVectorIterator<QString> i(*b);
    while(i.hasNext()){
        b_hold->append(i.next());
    }
    QVectorIterator<Token*> j(*t);
    while(j.hasNext()){
        t_hold->append(j.next());
    }

}

int Syn_analyzer::analyze(){
    //Token *t;
    Token* curToken;
    QString t;
    QString b;
    Grammar* g;
    QStack<QString> tmp;
    QStack<Token*> t_hold;
    QStack<QString> b_hold;
    char thrown = 0;
    char tok_changed = 0;
    QString tok_changed_buf = "";
    g = getGrammar("GRAMMAR_MAIN");

    QString start = buffer->pop();
    QStandardItem *startItem = new QStandardItem(start);
    rootNode->appendRow(startItem);
    buffer->push(start);
    QStack<QStandardItem*> items;
    items.push(startItem);
    QStandardItem *currentRoot;
    QMap<QPair<QString, QString>, QPair<QList<QString>*, int>* > *table = g->getTable();
    while(!tokens->isEmpty()){
        curToken = tokens->pop();
        t = curToken->toktype;
        qDebug() <<"tok: "<< t;
        thrown = 0;
        while(!thrown){
            if(!buffer->isEmpty()){
                b = buffer->pop();
                currentRoot = items.pop();
            }else{
                qDebug() <<"Error: buffer is empty and tokens are not";
                return 0;
            }
            qDebug() <<"buffer: "<< b;
            if(t == b && tok_changed_buf == ""){
                qDebug() <<"Throw!";
                thrown = 1;
//                QStandardItem *newItem = new QStandardItem(">>"+curToken->value);
//                currentRoot->appendRow(newItem);
                currentRoot->setToolTip(curToken->value);
            }
            else{
                if(t == b){
                    t = tok_changed_buf;
                    tok_changed_buf = "";
                }
                QPair<QString, QString> key(b, t);
                QPair<QList<QString>*, int>* rule = table->value(key);
                if(rule == NULL){
                    qDebug() <<"rule == NULL";
                    QPair<QString, QString> key_eps(b, "eps");
                    QPair<QList<QString>*, int>* rule_eps = table->value(key_eps);
                    if(rule_eps != NULL){
                        qDebug() <<"Throw " <<b <<" with eps rule!";
                        continue;
                    }
                    if(b == "eps"){
                        qDebug() <<"Throw " <<b <<" with eps rule!";
                        continue;
                    }
                    if(isGrammar(b)){
                        if(tok_changed){
                            t = tok_changed_buf;
                            tok_changed = 0;
                        }
                        tmp.push(g->getName());
                        qDebug() <<"Grammar " <<g->getName() <<"pushed." ;
                        //getwchar();
                        qDebug() <<"getting grammar:" <<b;
                        g = getGrammar(b);
                        if(g == NULL){
                            qDebug() <<"Error invalid grammar token!";
                            exit(1);
                        }else{
                            table = g->getTable();
                            QString cur = g->getStart();
                            buffer->push(cur);
                            items.push(currentRoot);
                            qDebug() <<"Inserting to buf: " << cur;
                            if(cur == "LE"){
                                //Token *tok = new Token;
                                //tok->id = -1;
                                //tok->tokclass = "temp";
                                //tok->toktype = "GRAMMAR_OPERATION";
                                //tok->value = "GRAMMAR_OPERATION";
                                //tokens->push(*tok);
                                tok_changed_buf = t;
                                t =  "GRAMMAR_OPERATION";
                                qDebug() <<"Inserting to tokens temporary: " << t;
                            }
                            /*if(cur == "OPERATION" && tok_changed_buf != ""){
                                t = tok_changed_buf;
                                tok_changed_buf = "";
                            }*/
                            //getwchar();
                            continue;
                        }
                    }else{
                        if(tmp.isEmpty()){
                            qDebug() <<"Error in parsing!";
                            exit(1);
                        }else{
                            /*QListIterator<QString> FIRST_of_element (*(g->getFIRST()->value(b)));
                            while(FIRST_of_element.hasNext()){
                                QString str = FIRST_of_element.next();
                                if(isGrammar(str)){
                                    copyStacks(tokens, &t_hold, buffer, &b_hold);
                                    tok_changed_buf = t;
                                    t = str;
                                    tok_changed = 1;
                                }
                            }*/


                            g = getGrammar(tmp.pop());
                            qDebug() <<"Grammar " <<g->getName() <<"poped." ;
                            table = g->getTable();
                            rule = table->value(key);
                            //getwchar();
                            qDebug() <<"tok, b = " <<t <<" " <<b;
                            if(rule == NULL){

                                buffer->push(b);
                                items.push(currentRoot);
                                continue;
                            }
                        }
                    }
                }
                qDebug() <<"rule != NULL";
                QStack<QStandardItem*> reverser;
                QListIterator<QString> i(*(rule->first));
                while(i.hasNext()){
                    //qDebug() <<"Iterating to the end of rule: " << i.next();
                    //getwchar();
                    QString cur = i.next();
                    QStandardItem *newItem = new QStandardItem(cur);
                    currentRoot->appendRow(newItem);
                    reverser.push(newItem);
                }
                while(i.hasPrevious()){
                    QString cur = i.previous();
                    QStandardItem *newItem = reverser.pop();
                    items.push(newItem);
                    buffer->push(cur);
                    qDebug() <<"Inserting to buf: " << cur;
                    //getwchar();
                }
            }
        }
    }
}

void Syn_analyzer::setGrammars(QList<Grammar*> *gr){

}
