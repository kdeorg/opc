#ifndef SEMANTICANALYSER_H
#define SEMANTICANALYSER_H

#include "includes.h"

class Node;
class SemanticAnalyser
{
    QStandardItem *root;
public:
    SemanticAnalyser(QStandardItem *root);
    void process(QStandardItem *target, QTextEdit *txtTriads);
    static void copy(QStandardItem *src, QStandardItem *dest);
    static void copy(QStandardItem *src, Node *dest);
    static Node * defineNode(QStandardItem *src);
};

#endif // SEMANTICANALYSER_H
