//#include "semanticanalyser.h"
#include "includes.h"

SemanticAnalyser::SemanticAnalyser(QStandardItem *root):root(root)
{
}

void SemanticAnalyser::process(QStandardItem *target,QTextEdit *txtTriads)
{
    Node *node = new Node();
    copy(target,node);
    SPAM(node->process(););
    node->process();
    QList<Triada*> *triads = new QList<Triada*>();
    SPAM(node->postProcess(triads););
    node->postProcess(triads);
    SPAM(success);
    Types *types = new Types();
    Vars *vars = new Vars();

    node->processTypes(types,vars);
    SPAM(processTypes end);
    node->processVariables(types,vars);
    SPAM(processVariables end);
    node->testType(types,vars);
    SPAM(testType end);

    node->draw(root);
    txtTriads->append("TYPES:");
    types->print(txtTriads);
    txtTriads->append("\nVARIABLES:");
    vars->print(txtTriads);
    txtTriads->append("\nTRIADS:");
    for (int i = 0; i < triads->size(); ++i) {
        Triada *cur = triads->at(i);
//        qDebug()<<cur->toString()<<endl;
        txtTriads->append(cur->toString());
    }
    SPAM(end)
}

void SemanticAnalyser::copy(QStandardItem *src, QStandardItem *dest)
{
    if(!src||!dest){
        exit(1);
    }

    for (int i = 0; i < src->rowCount(); ++i) {
        QStandardItem *startItem = new QStandardItem(src->child(i)->text());
        dest->appendRow(startItem);
    }
    qDebug()<<"src = "<<src->text()<<";     childsCount = "<<src->rowCount()<<endl;

    for (int i = 0; i < src->rowCount(); ++i) {
        copy(src->child(i),dest->child(i));
    }
}

void SemanticAnalyser::copy(QStandardItem *src, Node *dest)
{
    if(!src||!dest){
        exit(1);
    }

    for (int i = 0; i < src->rowCount(); ++i) {
        Node *child = SemanticAnalyser::defineNode(src->child(i));
        dest->addChild(child);
    }
//    qDebug()<<"copy src = "<<src->text()<<";     childsCount = "<<src->rowCount()<<endl;

    for (int i = 0; i < src->rowCount(); ++i) {
        copy(src->child(i),dest->getChild(i));
    }
}

#define QQ(argType,argName,argCompleteCount)\
    if(text==#argName){\
        node->type = argType;\
        node->completeCount=argCompleteCount;\
    }else
#define QQEND {\
    node->type = TermCommon;\
    node->completeCount=0;\
}
//REMEMBER ABOUT setFrom!!!!!!
Node *SemanticAnalyser::defineNode(QStandardItem *src)
{
    Node *node = new Node();
    QString text = src->text();
    node->name = text;
    node->value = src->toolTip();
    QQ(NoTerm,OPERATION,0)
    QQ(NoTerm,OPERAND_ADD,0)
    QQ(NoTerm,OPERATIONTALE,0)
    QQ(NoTerm,OPERAND_MUL,0)
    QQ(NoTerm,SPECO,0)
    QQ(NoTerm,OPCOMDEN,0)
            QQ(NoTerm,OPERAND_MUL_VTALE,0)
            QQ(NoTerm,OPERAND_MUL_TALE,0)
            QQ(NoTerm,OPERAND_MUL_VTALESP0,0)
            QQ(NoTerm,ARRNUMS,0)
            QQ(NoTerm,ARRNUM,0)
            QQ(NoTerm,ARRNUMTALE,0)
            QQ(NoTerm,OPERAND_MUL_UNS,0)

            QQ(NoTerm,LE,0)
            QQ(NoTerm,DISO,0)
            QQ(NoTerm,DISOTALE,0)
            QQ(NoTerm,CONJO,0)
            QQ(NoTerm,CONJOTALE,0)
            QQ(NoTerm,NEGO,0)
            QQ(NoTerm,RATOP,0)
            QQ(NoTerm,RATIOTALE,0)
            QQ(NoTerm,RATIOTALETALE,0)

            QQ(NoTerm,OSET,0)
            QQ(NoTerm,OSETTALE,0)
            QQ(NoTerm,O,0)
            QQ(NoTerm,OTALE,0)
            QQ(NoTerm,UNLO,0)
            QQ(NoTerm,UNOTCONDO,0)
            QQ(NoTerm,AO,0)
            QQ(NoTerm,FO,0)
            QQ(NoTerm,GO,0)
            QQ(NoTerm,INO,0)
            QQ(NoTerm,OUTO,0)
            QQ(NoTerm,COMO,0)
            QQ(NoTerm,UCONDO,0)
            QQ(NoTerm,MATCHED,0)
            QQ(NoTerm,MATCHEDTALE,0)
            QQ(NoTerm,MATCHEDTALETALE,0)
            QQ(NoTerm,OPTIONAL,0)
            QQ(NoTerm,TALE,0)
            QQ(NoTerm,TALETALE,0)
            QQ(NoTerm,TALETALETALE,0)
            QQ(NoTerm,NINO,0)
            QQ(NoTerm,INM,0)
            QQ(NoTerm,NOUTO,0)
            QQ(NoTerm,INMTALE,0)
            QQ(NoTerm,ARRNUMTALE,0)
            QQ(NoTerm,AOTALE,0)
            QQ(NoTerm,AOTALETALE,0)
            QQ(NoTerm,AOTALETALETALE,0)

            QQ(NoTerm,T,0)
            QQ(NoTerm,ST,0)
            QQ(NoTerm,UT,0)
            QQ(NoTerm,ARR,0)
            QQ(NoTerm,IDIAPS,0)
            QQ(NoTerm,IDIAP,0)
            QQ(NoTerm,IDIAPSTALE,0)
            QQ(NoTerm,TDIAP,0)
            QQ(NoTerm,ENUM,0)
            QQ(NoTerm,IDSET,0)
            QQ(NoTerm,IDSETTALE,0)
            QQ(NoTerm,TDIAPTALE,0)
            QQ(NoTerm,TDIAPTALETALE,0)
            QQ(NoTerm,TDIAPTALETALETALE,0)

            QQ(NoTerm,DV,0)
            QQ(NoTerm,VSET,0)
            QQ(NoTerm,VSETTALE,0)

            QQ(NoTerm,DL,0)
            QQ(NoTerm,LSET,0)
            QQ(NoTerm,LSETTALE,0)

            QQ(NoTerm,DC,0)
            QQ(NoTerm,IC,0)
            QQ(NoTerm,ICTALE,0)

            QQ(NoTerm,DT,0)

            QQ(NoTerm,S,0)
            QQ(NoTerm,N,0)
            QQ(NoTerm,SD,0)
            QQ(NoTerm,GRAMMAR_OPERATION,0)
            QQ(NoTerm,GRAMMAR_LOG,0)
            QQ(NoTerm,GRAMMAR_OPERATOR_SET,0)
            QQ(NoTerm,GRAMMAR_TYPES,0)
            QQ(NoTerm,GRAMMAR_VAR_DESCRIPTION,0)
            QQ(NoTerm,GRAMMAR_LAB_DESCRIPTION,0)
            QQ(NoTerm,GRAMMAR_CONST_DESCRIPTION,0)
            QQ(NoTerm,GRAMMAR_TYPE_DESCRIPTION,0)
            QQ(NoTerm,GRAMMAR_MAIN,0)
            QQ(NoTerm,OPERAND_ADDTALE,0)

            QQ(TermUseless,SSCOL0,0)// ;
            QQ(TermUseless,eps,0)

            QQ(TermUseless,SPC0,0)//   )
            QQ(TermUseless,KEN,0)//    end
            QQ(TermUseless,SP0,0)//    .
            QQ(TermUseless,SBKC0,0)//  ]

            QQ(TermOperator,SPO0,1)//   (
            QQ(TermOperator,SCOL0,0)// :
            QQ(TermOperator,SBKO0,2)// [

            QQ(TermOperator,KNUM,0)//  numerator
            QQ(TermOperator,KDEN,0)//  denominator
            QQ(TermOperator,KFRC,0)//  frac
            QQ(TermOperator,KIMP,0)//  improper
            QQ(TermOperator,KSIM,0)//  simple
            QQ(TermOperator,KCDN,0)//  comden

            QQ(TermOperator,KFR,7)//   for
            QQ(TermOperator,KTO,0)//   to
            QQ(TermOperator,KDO,0)//   do
            QQ(TermOperator,KGO,1)//   goto

            QQ(TermOperator,KIF,0)//   if
            QQ(TermOperator,KTH,0)//   then
            QQ(TermOperator,KEL,0)//   else
            QQ(TermOperator,KPR,0)//   program
            QQ(TermOperator,KBG,0)//   begin
            QQ(TermOperator,KOR,2)//   or
            QQ(TermOperator,KAND,2)//  and
            QQ(TermOperator,KNOT,1)//  not
            QQ(TermOperator,KRD,0)//   read
            QQ(TermOperator,KRDL,0)//  readln
            QQ(TermOperator,KWT,0)//   write
            QQ(TermOperator,KWTL,0)//  writeln
            QQ(TermOperator,KINT,0)//  int
            QQ(TermOperator,OPAR0,2)//     +
            QQ(TermOperator,OPAR1,2)//     -
            QQ(TermOperator,OPMUL0,2)//    *
            QQ(TermOperator,OPMUL1,2)//    /

            QQ(TermOperator,OPDPS0,2)//    ..
            QQ(TermOperator,OPA0,2)//      :=
            QQ(TermOperator,OPINIT0,2)//   =
            QQ(TermOperator,OPCP1,2)// >
            QQ(TermOperator,OPCP2,2)// <
            QQ(TermOperator,OPCP3,2)// <>
            QQ(TermOperator,OPCP4,2)// <=
            QQ(TermOperator,OPCP5,2)// >=
            QQ(TermOperator,SCOM0,2)//  ,
        QQEND
    return node;
}
