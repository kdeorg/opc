#include "grammar.h"

Grammar::Grammar(){
    terminals = new QList<QString>();
    nonterminals = new QList<QString>();
    rules = new QMap<int, Rule*>();
    start = "";
    name = "";

    FIRST = new QMap<QString, QList<QString>*>();

    table = new QMap<QPair<QString, QString>, QPair<QList<QString>*, int>* >();
}

Grammar::~Grammar(){
    delete terminals;
    delete nonterminals;
    QMap<int, Rule*>::iterator i;
    for(i = rules->begin(); i!=rules->end(); i++)
        delete i.value();
    delete rules;

    QMap<QString, QList<QString>*>::iterator j;
    for(j = FIRST->begin(); j!=FIRST->end(); j++)
        delete j.value();
    delete FIRST;
}

int Grammar::checkGrammar(){
    int ret = 1;
    //get all lexems from rules
    QList<QString>* inRules = new QList<QString>();
    QMap<int,Rule*>::iterator k;
    for(k = rules->begin(); k!=rules->end(); k++){
        QString left = k.value()->getLeft();
        if(!inRules->contains(left))
            inRules->append(left);
        QListIterator<QString> i (*k.value()->getRight());
        while(i.hasNext()){
            QString right = i.next();
            if(!inRules->contains(right))
                inRules->append(right);
        }

    }
//Check whether all lexems in rules are terminals, nonterminals or eps
    QListIterator<QString> i (*inRules);
    while(i.hasNext()){
        QString current = i.next();
        if(isTerminal(&current))
            continue;
        else if(isNonterminal(&current))
            continue;
        else if(current == "eps")
            continue;
        else{
            ret = 0;
            qDebug() <<"Grammar "<<name <<" error! Unexpected character: " <<current <<"in rules";
            break;
        }
    }
    //Check whether all lexems in terminals are used in rules
    QListIterator<QString> t (*terminals);
    while(t.hasNext()){
        QString current = t.next();
        if(!inRules->contains(current)){
            ret = 0;
            qDebug() <<"Grammar "<<name <<" error! terminal: " <<current <<"is not used in rules";
            break;
        }
    }
    //Check whether all lexems in nonterminals are used in rules
    QListIterator<QString> n (*nonterminals);
    while(n.hasNext()){
        QString current = n.next();
        if(!inRules->contains(current)){
            ret = 0;
            qDebug() <<"Grammar "<<name <<" error! nonterminal: " <<current <<"is not used in rules";
            break;
        }
    }
    //Check whether start symbol is used in rules
    if(!inRules->contains(start)){
        qDebug() <<"Grammar "<<name <<" error! start symbol is not used in rules";
        ret = 0;
    }
    //Check whether start symbol is nonterminal
    if(!nonterminals->contains(start)){
        qDebug() <<"Grammar "<<name <<" error! start symbol is not a nonterminal";
        ret = 0;
    }
    delete inRules;
}

int Grammar::addTerminal(QString* terminal){
    QString str = *terminal;
    QListIterator<QString> i (*terminals);
    while(i.hasNext()){
         if(i.next() == str){
             qDebug() << "Error! Terminal " <<str <<"is dublicated in" <<name ;
             return 0;
         }
     }
    terminals->append(str);
    return 1;
}

int Grammar::addNonterminal(QString* nonterminal){
    QString str = *nonterminal;
    QListIterator<QString> i (*nonterminals);
    while(i.hasNext()){
         if(i.next() == str){
             qDebug() << "Error! Nonterminal " <<str <<"is dublicated in" <<name ;
             return 0;
         }
     }
    nonterminals->append(str);
    return 1;
}

int Grammar::setStart(QString* start){
    this->start = *start;
}

void Grammar::setName(QString* name){
    this->name = *name;
}

int Grammar::addRule(int num, QList<QString>* right, QString* left){
    Rule* newrule = new Rule(num, left, right);
    //newrule->print();
    //getwchar();
    rules->insert(num, newrule);
}

QString Grammar::getStart(){
    return start;
}

QString Grammar::getName(){
    return name;
}

Rule* Grammar::getRule(int number){
    return rules->value(number);
}

QMap<QString, QList<QString>*> * Grammar::getFIRST(){
    return FIRST;
}

void Grammar::print(){
    qDebug() << this->name <<endl;

    QListIterator<QString> i (*nonterminals);
    qDebug() <<"Nonterminals: " <<endl;
    while(i.hasNext()){
        qDebug() << i.next() <<endl;
    }

    QListIterator<QString> j (*terminals);
    qDebug() <<"Terminals: " <<endl;
    while(j.hasNext()){
        qDebug() << j.next() <<endl;
    }

    QMap<int,Rule*>::iterator k;
    qDebug() <<"Rules: " <<endl;
    //while(k.hasNext()){
    //    k.value()->print();
    //}
    for(k = rules->begin(); k!=rules->end(); k++){
        k.value()->print();
    }

    qDebug() <<"Start: " << this->start <<endl;
}

int Grammar::isTerminal(QString* lex){
    if(terminals->contains(*lex))
       return 1;
    else
       return 0;
}

int Grammar::isNonterminal(QString* lex){
    if(nonterminals->contains(*lex))
       return 1;
    else
       return 0;
}

void Grammar::generateFIRST_SET(){
    QListIterator<QString> i (*nonterminals);
    while(i.hasNext()){
        QList<QString>* result = new QList<QString>();
        QString current = i.next();
        generateFIRST_SET_forNonterminal(&current, result);
        FIRST->insert(current, result);
    }
}

void Grammar::generateFIRST_SET_forNonterminal(QString* nonterminal, QList<QString>* result){
    QMap<int,Rule*>::iterator k;
    for(k = rules->begin(); k!=rules->end(); k++){
        if(k.value()->getLeft() == *nonterminal){
            QString first = k.value()->getRight()->at(0);
            if(isTerminal(&first)){
                if(!result->contains(first))
                    result->append(first);
            }else{
                if(isNonterminal(&first))
                    generateFIRST_SET_forNonterminal(&first, result);
            }
        }
    }
}

void Grammar::generateFIRST_SET_with_eps(){
    QListIterator<QString> i (*nonterminals);
    while(i.hasNext()){
        QList<QString>* result = new QList<QString>();
        QString current = i.next();
        generateFIRST_SET_forNonterminal_with_eps(&current, result);
        FIRST->insert(current, result);
    }
}

void Grammar::generateFIRST_SET_forNonterminal_with_eps(QString* nonterminal, QList<QString>* result){
    QMap<int,Rule*>::iterator k;
    for(k = rules->begin(); k!=rules->end(); k++){
        if(k.value()->getLeft() == *nonterminal){
            QString first = k.value()->getRight()->at(0);
            if(isTerminal(&first)||first == "eps"){
                if(!result->contains(first))
                    result->append(first);
            }else{
                if(isNonterminal(&first))
                    generateFIRST_SET_forNonterminal(&first, result);
            }
        }
    }
}

void Grammar::generateTable(){
    QListIterator<QString> n (*nonterminals);
    while(n.hasNext()){
        QString n_cur = n.next();
        QMap<int,Rule*>::iterator r;
        for(r = rules->begin(); r!=rules->end(); r++){
            if(r.value()->getLeft() == n_cur){
                QString right = r.value()->getRight()->at(0);
                if(isNonterminal(&right)){
                    QListIterator<QString> FIRST_of_right_element (*(FIRST->value(right)));
                    qDebug() <<"RIGHT:" <<right;
                    while(FIRST_of_right_element.hasNext()){
                        QString current_elem_from_FIRST = FIRST_of_right_element.next();
                        qDebug() << current_elem_from_FIRST;
                        //getwchar();
                        QPair<QString, QString> table_headers(n_cur, current_elem_from_FIRST);
                        QPair<QList<QString>*, int>* table_value = new QPair<QList<QString>*, int>
                                (r.value()->getRight(), r.value()->getId());
                        table->insert(table_headers, table_value);
                    }
                }else{

                    qDebug() <<"RIGHT:" <<right;
                    QPair<QString, QString> table_headers(n_cur, right);
                    QPair<QList<QString>*, int>* table_value = new QPair<QList<QString>*, int>
                            (r.value()->getRight(), r.value()->getId());
                    table->insert(table_headers, table_value);
                }
            }
        }

    }
    if(name == "GRAMMAR_MAIN"){
        QPair<QString, QString> table_headers("SD", "KBG");
        QList<QString>* eps = new QList<QString>();
        eps->append("eps");
        QPair<QList<QString>*, int>* table_value = new QPair<QList<QString>*, int>
                (eps, 7);
        table->insert(table_headers, table_value);
    }
    QListIterator<QString> j (*terminals);
    while(j.hasNext()){
        QString current = j.next();
        int throw_= -1;
        QPair<QString, QString> table_headers(current, current);
        QPair<QList<QString>*, int>* table_value = new QPair<QList<QString>*, int>
                (NULL, throw_);
        table->insert(table_headers, table_value);
    }
    int passed= -2;
    QPair<QString, QString> table_headers("finish", "eps");
    QPair<QList<QString>*, int>* table_value = new QPair<QList<QString>*, int>
            (NULL, passed);
    table->insert(table_headers, table_value);
}

QMap<QPair<QString, QString>, QPair<QList<QString>*, int>* > * Grammar::getTable(){
    return table;
}
