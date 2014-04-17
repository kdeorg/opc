#include "variable.h"

Variable::Variable(Node *node)
{
    this->name=node->value;
    value = "";
    constant = false;
    type = NULL;
}

void Variable::setType(Type *t)
{
    this->type=t;
}

void Variable::setIsConst(bool arg)
{
    constant = arg;
}

void Variable::setValue(int value)
{
    this->value=QString::number(value);
}

void Variable::setValue(QString value)
{
    this->value = value;
}

Vars::Vars()
{
}

void Vars::print(QTextEdit *txtTriads)
{
    for (int i = 0; i < this->size(); ++i) {
        Variable *cur = this->at(i);
        QString msg = "var "+QString::number(i)+") "+((cur->constant)?"const [":" [")+cur->name+","+cur->value+"]: "+cur->type->toString();
        txtTriads->append(msg);
    }
}

Variable *Vars::findVarByName(QString name)
{
    for (int i = 0; i < size(); ++i) {
        Variable *cur = at(i);
        if(cur->name==name){
            return cur;
        }
    }
    return NULL;
}

void Vars::append(Variable *arg)
{
    Variable *t = findVarByName(arg->name);
    if(t){
        qDebug()<<"try override variable "<<arg->name<<endl;
        EXCEPTION(true,Vars::append);
    }
    QList<Variable *>::append(arg);
}
