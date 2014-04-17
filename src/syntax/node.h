#ifndef NODE_H
#define NODE_H

#include "includes.h"

class Type;
class Types;
class ArrayType;
class Vars;
class Triada;
class Node
{
    static int idGenerator;
    static int indexGenerator;
public:
    static int triIndexGenerator;
    int id;
    QString name;
    QString value;
    NodeType type;
    int completeCount;
    Node *parent;
    QList<Node*> *childs;
    Type *varType;

    int triIndex;

    Node();
    ~Node();
    void process();
    void deleteChild(Node *child);
    void setFrom(Node *child);
    void addChild(Node *node);
    Node *getChild(int index);
    void removeNode(Node *node);

    bool isNoTerm();
    bool isUselessTerminal();
    bool hasNoTerm();
    Node *getNoTerminal();
    bool hasUselessTerminal();
    Node *getUselessTerminal();
    bool hasChildOperator();
    Node *getChildOperator();

    void draw(QStandardItem *dest);
    void draw(QStandardItem *dest, Node *cur);
    QString toString();
    bool containsNoTerminals();

    void postProcess(QList<Triada *> *list);
    bool postProcessRecursive();
    void moveUp(Node *child);
    void insertChildAfter(Node *arg, Node *child);
    void insertChildBefore(Node *arg, Node *child);
    int getChildPosition(Node *child);
    Node *getChild(QString name);
    bool hasChild(QString name);

    void processOperators();
    bool processAloneMinus(Node *c);
    bool isCompleteOperator();
    bool isAllOperatorsComplete();
    Node *getFirstIncompleteOperator();
    Node *getFirstCompleteOperatorFor(Node *node);
    Node *getBefore(Node *arg);
    Node *getAfter(Node *arg);
    void moveAllUp();
    bool hasOverflowedOperators();
    Node *getLastOverflowOperator();
    bool isOverflowOperator();
    Triada * processBegin(QList<Triada*> *list);
    void setTriIndexRecurse();
    Triada * fillTriadListRecursive(QList<Triada*> *list);
    Triada *processBool(QList<Triada*> *list);
    Triada *processIf(QList<Triada*> *list);
    Triada *processFor(QList<Triada*> *list);
    Triada *processOperation(QList<Triada*> *list);

    bool testType(Types *types,Vars *vars);
    void setOperationType(Types *types,Vars *vars);
    bool isCorrectType();
    void processTypes(Types *types,Vars *vars);
    void processSectionType(Types *types,Vars *vars);
    void addConstsForType(Type *t,Vars *vars);
    void addArrayDiapazons(ArrayType *t);
    void processVariables(Types *types,Vars *vars);
    void processConstSection(Types *types, Vars *vars);
    void processVarSection(Types *types, Vars *vars);
    Type *getTypeFromCurrentNode(Types *types, Vars *vars, int &iterator, QString typeName);
};

#endif // NODE_H
