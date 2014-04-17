#ifndef INCLUDES_H
#define INCLUDES_H

#define SPAM(arg) qDebug()<<"SPAM: "<<#arg<<endl;
#define EXCEPTION(arg,msg)   \
    if(arg){\
        qDebug()<<"EXCEPTION :: "#msg" :: "#arg<<endl;\
        throw std::exception();\
    }

enum NodeType{
    NoTerm,
    TermUseless,	//";",...
    TermCommon,		//"1","a",...
    TermOperator	//":=","+",...
};

#include <QApplication>
#include <QTreeView>
#include <QtGui>
#include <QGridLayout>
#include <QPushButton>
#include <QTextEdit>



#include "node.h"
#include "mainwindow.h"
#include "xmlreader.h"
#include "lex_analyzer.h"
#include "syn_analyzer.h"
#include "triada.h"
#include "type.h"
#include "variable.h"
#include "semanticanalyser.h"



#endif // INCLUDES_H
