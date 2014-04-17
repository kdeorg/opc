#include "type.h"

Type::Type(QString name)
{
    this->name = name;
}

void Type::setBaseType(Node *node)
{
    baseType = node->name;
}

void Type::setBaseType(QString str)
{
    baseType = str;
}

QString Type::toString()
{
    return name+": "+baseType;
}

bool Type::isSimple()
{
    return false;
}

bool Type::isRange()
{
    return false;
}

bool Type::isEnum()
{
    return false;
}

bool Type::isArray()
{
    return false;
}

int Types::nameGenerator=0;

Types::Types()
{
    SimpleType *integerType = new SimpleType("Integer");
    integerType->baseType = "KTINT0";
    append(integerType);
    SimpleType *rationalType = new SimpleType("Rational");
    rationalType->baseType = "KTRAT0";
    append(rationalType);
    SimpleType *labelType = new SimpleType("Label");
    labelType->baseType = "KLAB";
    append(labelType);
}

void Types::print(QTextEdit *txtTriads)
{
    for (int i = 0; i < this->size(); ++i) {
        Type *cur = this->at(i);
        QString msg = "type "+QString::number(i)+") "+cur->toString();
        txtTriads->append(msg);
    }
}

SimpleType *Types::getInteger()
{
    return (SimpleType *)at(0);
}

SimpleType *Types::getRational()
{
    return (SimpleType *)at(1);
}

QString Types::getFreeName()
{
    QString fName = "g"+QString::number(Types::nameGenerator++);
    while (findTypeByName(fName)!=NULL) {
        fName = "g"+QString::number(Types::nameGenerator++);
    }
    return fName;
}

Type *Types::findTypeByName(QString name)
{
    for (int i = 0; i < size(); ++i) {
        Type *cur = at(i);
        if(cur->name==name){
            return cur;
        }
    }
    return NULL;
}

void Types::append(Type *arg)
{
    Type *t = findTypeByName(arg->name);
    if(t){
        qDebug()<<"try override type "<<arg->name<<endl;
        EXCEPTION(true,Types::append);
    }
    QList<Type *>::append(arg);
}

SimpleType::SimpleType(QString name):Type(name)
{
}

QString SimpleType::toString()
{
    return "Simple "+Type::toString();
}

bool SimpleType::isSimple()
{
    return true;
}


RangeType::RangeType(QString name):Type(name)
{
    setBaseType("KTINT0");
    min="0";
    max="0";
}

void RangeType::setDiapazon(Node *min, Node *max)
{
    this->min = min->value;
    this->max = max->value;
}

QString RangeType::toString()
{
    return "Range "+Type::toString()+" min("+min+") max("+max+")";
}

bool RangeType::isRange()
{
    return true;
}

int EnumType::counter = 0;

EnumType::EnumType(QString name):Type(name)
{
    setBaseType("KTINT0");
}

QString EnumType::toString()
{
    return "Enum "+Type::toString();
}

bool EnumType::isEnum()
{
    return true;
}


ArrayType::ArrayType(QString name):Type(name)
{
    firsts = new QList<QString>();
    seconds = new QList<QString>();
}

void ArrayType::addDimension(Node *min, Node *max)
{
    firsts->append(min->value);
    seconds->append(max->value);
}

QString ArrayType::toString()
{
    QString dims = "";
    for (int i = 0; i < firsts->size(); ++i) {
        dims =dims+" ["+firsts->at(i)+".."+seconds->at(i)+"]";
    }
    return "ArrayType "+Type::toString()+dims;
}

bool ArrayType::isArray()
{
    return true;
}


