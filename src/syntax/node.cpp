#include "node.h"
#include "includes.h"

int Node::idGenerator=0;
int Node::indexGenerator=0;
int Node::triIndexGenerator=0;

Node::Node():parent(NULL),type(NoTerm),name("NoName"),value(""),
    completeCount(0),triIndex(0)
{
    childs = new QList<Node*>();
    id = idGenerator++;
    varType = NULL;
}

Node::~Node()
{
    for (int i = 0; i < childs->size(); ++i) {
        Node * child = childs->at(i);
        delete(child);
    }
    childs->clear();
}

void Node::process()
{
    if(this->name=="S"){
        Node *n = childs->at(0);
        if(n->name=="KPR"){
            childs->removeAt(0);
            delete(n);

            n = ((Node *)childs->at(0))->childs->at(0);
            ((Node *)childs->at(0))->childs->removeAt(0);
            delete(n);

            n = childs->at(0);
            childs->removeAt(0);
            delete(n);

            n = childs->at(0);
            childs->removeAt(0);
            delete(n);
        }
    }
    if(isUselessTerminal()){
        if(childs->size()==0){
            if(parent!=NULL){
                parent->deleteChild(this);
            }
        }else{
            qDebug()<<"Node::process()::(isUselessTerminal()&&childs->size()!=0)"<<endl;
            getwchar();
            exit(1);
        }
        return;
    }
    if(!isNoTerm()){
        return;
    }
    if(childs->size()==0){
        if(parent!=NULL){
            parent->deleteChild(this);
        }
        return;
    }
    if(childs->size()==1){
        Node * child = childs->at(0);
        //replace this to child;
        this->setFrom(child);
        removeNode(child);
        child->childs->clear();
        delete(child);
        this->process();
        return;
    }
    if(hasNoTerm()){
        Node *noTerm = getNoTerminal();
        noTerm->process();
        this->process();
        return;
    }
    if(hasUselessTerminal()){
        Node *useless = getUselessTerminal();
        useless->process();
        this->process();
        return;
    }
    if(hasChildOperator()){
        processOperators();
    }else{
        while(childs->size()>0){
            Node * child = childs->first();
            moveUp(child);
        }
    }
}

void Node::processOperators(){
    QString names[5] = {"KTYP","KBG","KCON","KVAR","KEN"};
    for (int i = 0; i < 5; ++i) {
        if(hasChild(names[i])){
            Node * n = getChild(names[i]);
            if(n->childs->size()==0){
                Node *after = getAfter(n);
                while(after && !(after->name=="KBG"||
                                after->name=="KTYP"||
                                after->name=="KCON"||
                                after->name=="KVAR")){
                    removeNode(after);
                    n->addChild(after);
                    after = getAfter(n);
                }
            }
        }
    }

    if(hasChild("KIF")){
        Node * nIf = getChild("KIF");
        if(nIf->childs->size()==0){
            EXCEPTION(!nIf,processOperators)
            Node * nCond = getAfter(nIf);
            EXCEPTION(!nCond,processOperators)
            Node * nThen = getAfter(nCond);
            EXCEPTION(!nThen,processOperators)
            Node * nOperT = getAfter(nThen);
            EXCEPTION(!nOperT,processOperators)
            Node * nElse = getAfter(nOperT);
            Node * nOperE=NULL;
            if(nElse){
               nOperE = getAfter(nElse);
               EXCEPTION(!nOperE,processOperators)
            }
            removeNode(nCond);
            nIf->addChild(nCond);
            removeNode(nThen);
            nIf->addChild(nThen);
            removeNode(nOperT);
            nIf->addChild(nOperT);
            if(nElse){
                removeNode(nElse);
                nIf->addChild(nElse);
                removeNode(nOperE);
                nIf->addChild(nOperE);
            }
        }
    }
    if(hasChild("KFR")){
        Node * n = getChild("KFR");
        if(n->childs->size()==0){
            Node *after = getAfter(n);
            while(after && after->name!="KDO"){
                removeNode(after);
                n->addChild(after);
                after = getAfter(n);
            }
            removeNode(after);//KDO
            n->addChild(after);
            after = getAfter(n);
            removeNode(after);//operator after KDO
            n->addChild(after);
            after = getAfter(n);
        }
    }
    if(hasChild("SCOL0")){
        Node * n = getChild("SCOL0");
        if(n->childs->size()==0){
            Node * nId = getBefore(n);
            if(nId){
                removeNode(nId);
                n->addChild(nId);
            }
        }
    }
    /*
    if(hasChild("SCOM0")){
        Node * n = getChild("SCOM0");
        if(!n->isCompleteOperator()){
            if(n->childs->size()==0){
                Node *before = getBefore(n);
                Node *after = getAfter(n);
                if(before!=NULL && after!=NULL){
                    //movr before and after to c;
                    removeNode(before);
                    removeNode(after);
                    n->addChild(before);
                    n->addChild(after);
                    EXCEPTION(n->isCompleteOperator()==false,Node::processOperators-1);
                }else{
                    if(parent){
                        moveAllUp();
                    }
                    return;
                }
            }else *//*if(n->childs->size()==1){
                Node *before = getBefore(n);
                if(before!=NULL){
                    removeNode(before);
                    n->addChild(before);
                    EXCEPTION(n->isCompleteOperator()==false,Node::processOperators-2);
                }else{
                    if(parent){
                        moveAllUp();
                    }
                    return;
                }
            }else*//*{
                EXCEPTION(true,n->childs->size()>0);
            }
        }
    }*/
    if(hasChild("KNUM")||hasChild("KDEN")){
        Node * n = NULL;
        if(hasChild("KNUM")){
            n = getChild("KNUM");
        }if(hasChild("KDEN")){
            n = getChild("KDEN");
        }
        if(n->childs->size()==0){
            Node * nId = getBefore(n);
            if(nId){
                removeNode(nId);
                n->addChild(nId);
            }else{
                moveAllUp();
            }
        }
    }
    while(hasOverflowedOperators() || !isAllOperatorsComplete()){
        while(hasOverflowedOperators()){
            Node *c = getLastOverflowOperator();
            if(c->childs->size()>0){
                Node * last = c->childs->at(c->childs->size()-1);
                c->moveUp(last);
            }
        }
        while(!isAllOperatorsComplete()){
            Node *c = getFirstIncompleteOperator();
            EXCEPTION(c==NULL,Node::processOperators);
            if(hasChild("SCOM0")){
                Node *sc = getChild("SCOM0");
                if(!sc->isCompleteOperator()){
                    c=sc;
                }
            }
            if(     c->name=="SBKO0"||//    [
                    c->name=="OPAR0"||//    +
                    c->name=="OPAR1"||//    -
                    c->name=="OPMUL0"||//   *
                    c->name=="OPMUL1"||//   /
                    c->name=="OPCP1"||//    >
                    c->name=="OPCP2"||//    <
                    c->name=="OPCP3"||//    <>
                    c->name=="OPCP4"||//    <=
                    c->name=="OPCP5"||//    >=
                    c->name=="OPINIT0"||//  =
                    c->name=="KOR"||//      or
                    c->name=="KAND"||//     and
                    c->name=="SCOM0"||//     ,
                    c->name=="OPA0"//       :=
                    ){
    //            EXCEPTION(c->childs->size()!=0,Node::processOperators);

                if(c->childs->size()==0){
                    Node *before = getBefore(c);
                    Node *after = getAfter(c);
                    if(before!=NULL && after!=NULL){
                        //movr before and after to c;
                        removeNode(before);
                        removeNode(after);
                        c->addChild(before);
                        c->addChild(after);
                        EXCEPTION(c->isCompleteOperator()==false,Node::processOperators0);
                        continue;
                    }else{
                        if(processAloneMinus(c)){
                            continue;
                        }
                        if(parent){
                            moveAllUp();
                        }
                        return;
                    }
                }else if(c->childs->size()==1){
                    Node *before = getBefore(c);
                    if(before!=NULL){
                        removeNode(before);
                        c->addChild(before);
                        EXCEPTION(c->isCompleteOperator()==false,Node::processOperators1);
                        continue;
                    }else{
                        if(processAloneMinus(c)){
                            continue;
                        }
                        if(parent){
                            moveAllUp();
                        }
                        return;
                    }
                }else{
                    EXCEPTION(true,c->childs->size()>1);
                }


            }else{
                Node *complete = getFirstCompleteOperatorFor(c);
                if(complete){
                    //move complete to c
                    removeNode(complete);
                    c->addChild(complete);
                    continue;
                }else{
                    if(parent){
                        moveUp(c);
                    }
                    continue;
                }
            }
        }
    }
    if(     hasChild("KWT")||hasChild("KWTL")||hasChild("KRD")||hasChild("KRDL")
            ||hasChild("KFRC")||hasChild("KIMP")||hasChild("KSIM")||hasChild("KCDN")
            ||hasChild("KINT")){
        Node * n = NULL;
        if(hasChild("KWT")){
            n = getChild("KWT");
        }else if(hasChild("KWTL")){
            n = getChild("KWTL");
        }else if(hasChild("KRD")){
            n = getChild("KRD");
        }else if(hasChild("KRDL")){
            n = getChild("KRDL");
        }else if(hasChild("KFRC")){
            n = getChild("KFRC");
        }else if(hasChild("KIMP")){
            n = getChild("KIMP");
        }else if(hasChild("KSIM")){
            n = getChild("KSIM");
        }else if(hasChild("KCDN")){
            n = getChild("KCDN");
        }else if(hasChild("KINT")){
            n = getChild("KINT");
        }

        if(n->childs->size()==0){
            Node * after1 = getAfter(n);
            EXCEPTION(!after1,processOperators);
            removeNode(after1);
            n->addChild(after1);
        }
    }
    if(parent){
        moveAllUp();
    }
}

bool Node::processAloneMinus(Node *c)
{
    if(     c->name=="OPAR0"||//    +
            c->name=="OPAR1"//      -
            ){
        Node *beforeParent = parent->getBefore(this);
        EXCEPTION(!beforeParent,parent->getBefore(this))
                if(beforeParent->isCompleteOperator()==false&&(
                       beforeParent->name=="OPA0"||//     :=
                       beforeParent->name=="SBKO0"||//    [
                       beforeParent->name=="SPO0"||//     (
                       beforeParent->name=="OPCP1"||//    >
                       beforeParent->name=="OPCP2"||//    <
                       beforeParent->name=="OPCP3"||//    <>
                       beforeParent->name=="OPCP4"||//    <=
                       beforeParent->name=="OPCP5"||//    >=
                       beforeParent->name=="OPINIT0"//    =
                       )||
                   beforeParent->name=="KTO"//      to
                   ){
            Node *zeroNode = new Node();
            zeroNode->name="CINT";
            zeroNode->value="0";
            zeroNode->type = TermCommon;
            zeroNode->completeCount=0;
            insertChildBefore(c,zeroNode);
            return true;
        }
    }
    return false;
}

bool Node::isCompleteOperator()
{
    if(type==TermOperator){
        int size=childs->size();
        if(size>=completeCount){
            return true;
        }else{
//            qDebug()<<"isCompleteOperator :: "<<name<<".size = "<<size<<endl;
            return false;
        }
    }
    if(type==TermCommon){
        return true;
    }
    return false;
}

bool Node::isAllOperatorsComplete()
{
    for (int i = 0; i < childs->size(); ++i) {
        Node * cur = childs->at(i);
        if(cur->type==TermOperator && !cur->isCompleteOperator()){
            return false;
        }
    }
    return true;
}

Node *Node::getFirstIncompleteOperator()
{
    for (int i = 0; i < childs->size(); ++i) {
        Node * cur = childs->at(i);
        if(cur->type==TermOperator && !cur->isCompleteOperator()){
            return cur;
        }
    }
    return NULL;
}

Node *Node::getFirstCompleteOperatorFor(Node *node)
{
    for (int i = childs->size()-1; i >= 0; --i) {
        Node * cur = childs->at(i);
        if(cur->type==TermCommon){
            return cur;
        }
        if(cur->type==TermOperator && cur->isCompleteOperator()){
            if(     node->name=="OPA0" ||// :=
                    node->name=="SPO0" ||// (
                    node->name=="OPAR0" ||//+
                    node->name=="SCOM0" ||// ,
                    node->name=="OPAR1"//   -
                    ){
                if(     cur->name=="OPAR0"||
                        cur->name=="OPAR1"||
                        cur->name=="OPMUL0"||
                        cur->name=="OPMUL1"||
                        cur->name=="KNUM"||
                        cur->name=="KDEN"||
                        cur->name=="KFRC"||
                        cur->name=="KIMP"||
                        cur->name=="KSIM"||
                        cur->name=="KCDN"||
                        cur->name=="SPO0"||
                        cur->name=="SCOM0"||
                        cur->name=="KINT"||
                        cur->name=="SBKO0"
                        ){
                    return cur;
                }else{
                    return NULL;
                }
            }else if(node->name=="OPMUL0" ||//  *
                     node->name=="OPMUL1"//     /
                     ){
                if(     cur->name=="OPAR0"||
                        cur->name=="OPAR1"||
                        cur->name=="OPMUL0"||
                        cur->name=="OPMUL1"||
                        cur->name=="KNUM"||
                        cur->name=="KDEN"||
                        cur->name=="KFRC"||
                        cur->name=="KIMP"||
                        cur->name=="KSIM"||
                        cur->name=="KCDN"||
                        cur->name=="SPO0"||
                        cur->name=="SCOM0"||
                        cur->name=="KINT"||
                        cur->name=="SBKO0"
                        ){
                    return cur;
                }else{
                    return NULL;
                }
            }else if(node->name=="KNUM" ||//numerator
                     node->name=="KDEN" ||//denominator
                     node->name=="KFRC" ||//frac
                     node->name=="KIMP" ||//improper
                     node->name=="KSIM" ||//simple
                     node->name=="KCDN" //  comden
            ){
                if(     cur->name=="SBKO0"
                        ){
                    return cur;
                }else{
                    return NULL;
                }
            }else if(node->name=="KBG" ||// begin
                     node->name=="KTH" ||// then
                     node->name=="KEL" //   else
                    ){
                if(     cur->name=="KBG"||
                        cur->name=="KIF"||
                        cur->name=="KFR"||
                        cur->name=="KRD"||
                        cur->name=="KRDL"||
                        cur->name=="KWT"||
                        cur->name=="KWTL"||
                        cur->name=="OPA0"
                        ){
                    return cur;
                }else{
                    return NULL;
                }
            }else if(node->name=="KIF"//if
                    ){
                if(     cur->name=="OPINIT0"||
                        cur->name=="OPCP1"||
                        cur->name=="OPCP2"||
                        cur->name=="OPCP3"||
                        cur->name=="OPCP4"||
                        cur->name=="OPCP5"||
                        cur->name=="KOR"||
                        cur->name=="KAND"||
                        cur->name=="KNOT"||
                        cur->name=="KTH"||
                        cur->name=="KEL"
                        ){
                    return cur;
                }else{
                    return NULL;
                }
            }else if(node->name=="KFR"//for
                    ){
                if(     cur->name=="KBG"||
                        cur->name=="KIF"||
                        cur->name=="KFR"||
                        cur->name=="KRD"||
                        cur->name=="KRDL"||
                        cur->name=="KWT"||
                        cur->name=="KWTL"||
                        cur->name=="SPO0"||
                        cur->name=="SBKO0"||
                        cur->name=="KINT"||
                        cur->name=="OPA0"
                        ){
                    return cur;
                }else{
                    return NULL;
                }
            }else if(node->name=="KINT"||//
                     node->name=="KWT"||//
                     node->name=="KWTL"||//writeln
                     node->name=="KRD"||//
                     node->name=="KRDL"||//
                     node->name=="KFRC"||//frac
                     node->name=="KIMP"||//improper
                     node->name=="KSIM"||//simple
                     node->name=="KCDN"//comden
                    ){
                if(     cur->name=="SPO0"
                        ){
                    return cur;
                }else{
                    return NULL;
                }
            }else{
                return cur;//exception
            }
        }
    }
    return NULL;
}

Node *Node::getBefore(Node *arg)
{
    for (int i = 0; i < childs->size(); ++i) {
        Node * cur = childs->at(i);
        if(cur->id==arg->id){
            i--;
            if(i<0||(childs->size()>0 && i>childs->size()-1)||(childs->size()==0 && i!=0)){
                return NULL;
            }else{
                return childs->at(i);
            }
        }
    }
    return NULL;
}

Node *Node::getAfter(Node *arg)
{
    for (int i = 0; i < childs->size(); ++i) {
        Node * cur = childs->at(i);
        if(cur->id==arg->id){
            i++;
            if(i<0||(childs->size()>0 && i>childs->size()-1)||(childs->size()==0 && i!=0)){
                return NULL;
            }else{
                return childs->at(i);
            }
        }
    }
    return NULL;
}

void Node::moveAllUp()
{
    int size=childs->size();
    for (int i = size-1; i >= 0; --i) {
        Node * cur = childs->at(i);
        moveUp(cur);
    }
}

bool Node::hasOverflowedOperators()
{
    for (int i = 0; i < childs->size(); ++i) {
        Node * cur = childs->at(i);
        if(cur->type==TermOperator && cur->isOverflowOperator()){
            return true;
        }
    }
    return false;
}

Node *Node::getLastOverflowOperator()
{
    for (int i = childs->size()-1; i >=0 ; --i) {
        Node * cur = childs->at(i);
        if(cur->type==TermOperator && cur->isOverflowOperator()){
            return cur;
        }
    }
    return NULL;
}

bool Node::isOverflowOperator()
{
    if(     name=="KBG"||
            name=="KIF"||
            name=="KFR"||
            name=="KRD"||
            name=="KRDL"||
            name=="KWT"||
            name=="KWTL"||
            name=="KINT"||
            name=="KFRC"||
            name=="KIMP"||
            name=="KSIM"||
            name=="KCDN"||
            name=="SCOL0"||
            name=="KNUM"||
            name=="KDEN"
            ){
        return false;
    }

    if(type==TermOperator){
        return childs->size()>completeCount;
    }
    return false;
}

void Node::deleteChild(Node *child)
{
    bool wasDeleted=false;
    for (int i = 0; i < childs->size(); ++i) {
        Node * cur = childs->at(i);
        if(cur->id==child->id){
            childs->removeAt(i);
            wasDeleted = true;
            delete(cur);
        }
    }
    if(!wasDeleted){
        qDebug()<<"Node::deleteChild()::!wasDeleted"<<endl;
        getwchar();
        exit(1);
    }
}

void Node::setFrom(Node *child)
{
    this->name=child->name;
    this->value = child->value;
    this->type = child->type;
    this->completeCount=child->completeCount;
    int childPos = getChildPosition(child);
    if(childPos==-1){
        qDebug()<<"Node::setFrom()::childPos==-1"<<endl;
        getwchar();
        exit(1);
    }
    for (int i = 0; i < child->childs->size(); ++i) {
        Node * cur = child->childs->at(i);
        cur->parent = this;
        this->childs->insert(childPos+i,cur);
    }
}

void Node::addChild(Node *node)
{
    node->parent=this;
    childs->append(node);
}

Node *Node::getChild(int index)
{
    return childs->at(index);
}

void Node::removeNode(Node *node)
{
    for (int i = 0; i < childs->size(); ++i) {
        Node * cur = childs->at(i);
        if(cur->id==node->id){
            childs->removeAt(i);
            return;
        }
    }
    qDebug()<<"Node::removeNode()::!wasDeleted"<<endl;
    getwchar();
    exit(1);
}

bool Node::isNoTerm()
{
    return type==NoTerm;
}

bool Node::isUselessTerminal()
{
    return type==TermUseless;
}

bool Node::hasNoTerm()
{
    for (int i = 0; i < childs->size(); ++i) {
        Node * cur = childs->at(i);
        if(cur->type==NoTerm){
            return true;
        }
    }
    return false;
}

Node *Node::getNoTerminal()
{
    for (int i = 0; i < childs->size(); ++i) {
        Node * cur = childs->at(i);
        if(cur->type==NoTerm){
            return cur;
        }
    }
    qDebug()<<"Node::getNoTerminal()"<<endl;
    getwchar();
    exit(1);
    return NULL;
}

bool Node::hasUselessTerminal()
{
    for (int i = 0; i < childs->size(); ++i) {
        Node * cur = childs->at(i);
        if(cur->type==TermUseless){
            return true;
        }
    }
    return false;
}

Node *Node::getUselessTerminal()
{
    for (int i = 0; i < childs->size(); ++i) {
        Node * cur = childs->at(i);
        if(cur->type==TermUseless){
            return cur;
        }
    }
    qDebug()<<"Node::getUselessTerminal()"<<endl;
    getwchar();
    exit(1);
    return NULL;
}

void Node::draw(QStandardItem *dest)
{
    QStandardItem *startItem = new QStandardItem(this->toString());
    dest->appendRow(startItem);
    draw(startItem,this);
}

void Node::draw(QStandardItem *dest, Node *cur)
{
    for (int i = 0; i < cur->childs->size(); ++i) {
        Node *curChild = cur->childs->at(i);
        QStandardItem *startItem = new QStandardItem(curChild->toString());
        dest->appendRow(startItem);
        draw(dest->child(i),curChild);
    }
}

QString Node::toString(){
    QString typeS = "";
    if(type==NoTerm){
        typeS = "NoTerm";
    }else if(type==TermUseless){
        typeS = "TermUseless";
    }else if(type==TermCommon){
        typeS = "TermCommon";
    }else if(type==TermOperator){
        typeS = "TermOperator";
    }
    /*
    QString typeOp = "";
    if(operationType == TypeUnknown){
        typeOp+="|TypeUnknown";
    }else if(operationType & TypeInt){
        typeOp+="|TypeInt";
    }else if(operationType & TypeRat){
        typeOp+="|TypeRat";
    }else if(operationType & TypeLabel){
        typeOp+="|TypeLabel";
    }
*/
    return name
            +" value("+value+")"
            +"  id("+QString::number(id)+")"
            +"  tri("+QString::number(triIndex)+")"
            +"  type("+(varType?varType->name:"nullType")+")";
//            +" cc("+QString::number(completeCount)+")"
//            +" pt("+typeOp+")";
//            +" type("+typeS+")";
}

bool Node::containsNoTerminals()
{
    if(this->type==NoTerm){
        return true;
    }
    for (int i = 0; i < childs->size(); ++i) {
        Node *curChild = childs->at(i);
        if(curChild->containsNoTerminals()){
            return true;
        }
    }
    return false;
}

bool Node::hasChildOperator()
{
    for (int i = 0; i < childs->size(); ++i) {
        Node *cur = childs->at(i);
        if(cur->type==TermOperator){
            return true;
        }
    }
    return false;
}

Node *Node::getChildOperator()
{
//    if(hasChild("OPA0")){
//        return getChild("OPA0");
//    }
    for (int i = 0; i < childs->size(); ++i) {
        Node *cur = childs->at(i);
        if(cur->type==TermOperator){
            childs->at(i);
            return cur;
        }
    }
    qDebug()<<"Node::getChildOperator()"<<endl;
    getwchar();
    exit(1);
    return NULL;
}

void Node::postProcess(QList<Triada*> *list)
{
    if(name=="KBG"){
        processBegin(list);
    }else{
        for (int i = 0; i < childs->size(); ++i) {
            Node *cur = childs->at(i);
            cur->postProcess(list);
        }
    }
}

Triada *Node::processBegin(QList<Triada*> *list)
{
    for (int i = 0; i < childs->size(); ++i) {
        Node *cur = childs->at(i);
        Triada * result = cur->processOperation(list);
        if(i==childs->size()-1){
            return result;
        }
    }
    return 0;
}

void Node::setTriIndexRecurse(){
    if(childs->size()>0){
        triIndex = triIndexGenerator;
        triIndexGenerator++;
    }
    for (int i = 0; i < childs->size(); ++i) {
        Node *cur = childs->at(i);
        cur->setTriIndexRecurse();
    }
}

Triada * Node::fillTriadListRecursive(QList<Triada*> *list){
    Triada *result = new Triada(this);
    if(childs->size()>0){
        Node *arg = childs->at(0);
        if(arg){
            if(arg->childs->size()==0){
                result->setArg(0,arg);
            }else{
                result->setArg(0,arg->fillTriadListRecursive(list));
            }
        }
    }
    if(childs->size()>1){
        Node *arg = childs->at(1);
        if(arg){
            if(arg->childs->size()==0){
                result->setArg(1,arg);
            }else{
                result->setArg(1,arg->fillTriadListRecursive(list));
            }
        }
    }
    result->addToList(list);
    return result;
}

Triada *Node::processBool(QList<Triada *> *list)
{
    setTriIndexRecurse();
    return  fillTriadListRecursive(list);
}

Triada *Node::processIf(QList<Triada *> *list)
{
    if(!(childs->size()==5||childs->size()==3)){
        qDebug()<<childs->size()<<"  "<<toString();
        return 0;
    }
    EXCEPTION(!(childs->size()==5||childs->size()==3),processIf);
    triIndex = triIndexGenerator++;

    Triada *lElse = Triada::label("lElse");
    Triada *lEnd = Triada::label("lEnd");
    Triada *ifZeroGoTo = Triada::ifZeroGoTo();
    Triada *goToEnd = Triada::goTo("lEnd");

    Triada *boolTriada = ((Node *)childs->at(0))->processBool(list);

    ifZeroGoTo->addToList(list);

    Node *cur = childs->at(2);
    cur->processOperation(list);

    goToEnd->addToList(list);
    lElse->addToList(list);

    if(childs->size()==5){
        cur = childs->at(4);
        cur->processOperation(list);
    }

    lEnd->addToList(list);

    goToEnd->setArg(0,lEnd);
    ifZeroGoTo->setArg(0,boolTriada);
    ifZeroGoTo->setArg(1,lElse);
    return lEnd;
}

Triada *Node::processFor(QList<Triada *> *list)
{
    if(childs->size()!=7){
        qDebug()<<childs->size()<<"  "<<toString();
        return 0;
    }
    EXCEPTION(childs->size()!=7,Node::processFor)

    triIndex = triIndexGenerator++;

    Triada *topOperation = ((Node *)childs->at(4))->processOperation(list);


    Triada *initOperation = ((Node *)childs->at(2))->processOperation(list);

    Node *counter = childs->at(0);

    Triada *trInit = new Triada(":=");
    trInit->setArg(0,counter);
    trInit->setArg(1,initOperation);
    trInit->addToList(list);


    Triada *labelBegin = Triada::label("lBegin");
    labelBegin->addToList(list);

    Triada *trSub = new Triada("-");
    trSub->setArg(0,counter);
    trSub->setArg(1,topOperation);
    trSub->addToList(list);

    Triada *ifZeroGoTo = Triada::ifZeroGoTo();
    ifZeroGoTo->addToList(list);

    Node *body = childs->at(6);
    body->processOperation(list);

    Triada *trAdd = new Triada("increment");
    trAdd->setArg(0,counter);
    trAdd->addToList(list);

    Triada *trIncrement = new Triada(":=");
    trIncrement->setArg(0,counter);
    trIncrement->setArg(1,trAdd);
    trIncrement->addToList(list);

    Triada *gotoBegin = Triada::goTo("lBegin");
    gotoBegin->setArg(0,labelBegin);
    gotoBegin->addToList(list);

    Triada *lblEnd = Triada::label("lEnd");
    lblEnd->addToList(list);

    ifZeroGoTo->setArg(0,lblEnd);

    return lblEnd;
}

Triada *Node::processOperation(QList<Triada *> *list)
{
    if(     name=="KBG"//      begin
            ){
        return processBegin(list);
    }else if( name=="OPA0"//   :=
            ||name=="KRD"
            ||name=="KRDL"
            ||name=="KWT"
            ||name=="KWTL"
            ||name=="KGO"//    goto
            ||name=="SCOL0"//  :
              ||name=="OPAR0"//  +
              ||name=="OPAR1"//  -
              ||name=="OPMUL0"//  *
              ||name=="KOR"//  or
              ||name=="KAND"//  and
              ||name=="KNOT"//  not
              ||name=="OPINIT0"//  =
              ||name=="OPCP1"//  >
              ||name=="OPCP2"//  <
              ||name=="OPCP3"//  <>
              ||name=="OPCP4"//  <=
              ||name=="OPCP5"//  >=
            ){
        setTriIndexRecurse();
        return  fillTriadListRecursive(list);
    }else if(     name=="KIF"//if
                  ){
        return  processIf(list);
    }else if(     name=="KFR"//for
                  ){
        return  processFor(list);
    }else if(     type==TermCommon
                  ){
        Triada *c = new Triada(">access");
        c->setArg(0,this);
        c->addToList(list);
        return  c;
    }else{
        for (int i = 0; i < list->size(); ++i) {
            Triada *cur = list->at(i);
            qDebug()<<cur->toString()<<endl;
        }
        qDebug()<<"unknown: "<<toString()<<endl;
//        EXCEPTION(true,unknown operator);
        return 0;
    }
}

bool Node::testType(Types *types,Vars *vars)
{
    for (int i = 0; i < childs->size(); ++i) {
        Node *cur = childs->at(i);
        if(cur->name=="KBG"){
            cur->setOperationType(types,vars);
            return isCorrectType();
        }
    }
    EXCEPTION(true,testType);
    return false;
}

void Node::setOperationType(Types *types,Vars *vars)
{
    if(childs->size()==0){
        if(name=="ID"){
            Variable *var = vars->findVarByName(value);
            if(var){
                varType=var->type;
            }else{
                qDebug()<<"variable "<<value<<" not declared "<<endl;
                EXCEPTION(true,setOperationType)
            }
        }else if(name=="CINT"){
            varType=types->getInteger();
        }
    }else{
        for (int i = 0; i < childs->size(); ++i) {
            Node *cur = childs->at(i);
            cur->setOperationType(types,vars);
        }
        if(     name=="SCOM0"//,
                ||name=="OPA0"//:=
                ){
            Type *t1 = ((Node *)childs->at(0))->varType;
            Type *t2 = ((Node *)childs->at(1))->varType;
            EXCEPTION(t1!=t2,setOperationType 1);
            varType=t1;
        }else if(name=="OPCP5"//>=
                 ||name=="OPCP4"//<=
                 ||name=="OPCP3"//<>
                 ||name=="OPCP2"//<
                 ||name=="OPCP1"//>
                 ||name=="OPINIT0"//=

                 ||name=="KOR"//or
                 ||name=="KAND"//and

                 ||name=="OPAR0"//+
                 ||name=="OPAR1"//-
                 ||name=="OPMUL0"//*
                 ||name=="OPMUL1"// /
                 ){
            Type *t1 = ((Node *)childs->at(0))->varType;
            Type *t2 = ((Node *)childs->at(1))->varType;
            EXCEPTION(t1!=t2,setOperationType 2);
            EXCEPTION(!(t1->isSimple()||t1->isRange()||t1->isEnum()),setOperationType 3);
            EXCEPTION(t1->baseType!="KTINT0",setOperationType 4);
            varType=t1;
        }else if(name=="KNUM"//numerator
                 ||name=="KDEN"//denominator
                 ||name=="KFRC"//frac
                 ||name=="KIMP"//improper
                 ||name=="KSIM"//simple
            ){
            Type *t1 = ((Node *)childs->at(0))->varType;
            EXCEPTION(!t1,setOperationType 7);
            EXCEPTION(t1->baseType!="KTRAT0",setOperationType 5);
            EXCEPTION(!(t1->isSimple()||t1->isRange()||t1->isEnum()),setOperationType 6);
            varType=types->getInteger();
        }else if(name=="SPO0"//(
                     ){
            Type *t1 = ((Node *)childs->at(0))->varType;
            EXCEPTION(!t1,setOperationType 8);
            varType=t1;
        }else if(name=="SCOL0"//:
                 ||name=="KGO"//:
                     ){
            Type *t1 = ((Node *)childs->at(0))->varType;
            EXCEPTION(!t1,setOperationType 9);
            EXCEPTION(t1!=types->findTypeByName("Label"),setOperationType 10);
            varType=t1;
        }else if(name=="SBKO0"//[
                     ){
            Type *t1 = ((Node *)childs->at(0))->varType;
            Type *t2 = ((Node *)childs->at(1))->varType;
            EXCEPTION(!t1||!t2,setOperationType 11);
            EXCEPTION(!(t1->isArray()),setOperationType 12);
            EXCEPTION(!(t2->isSimple()),setOperationType 13);
            EXCEPTION(t2!=types->getInteger(),setOperationType 14);
            if(t1->baseType=="KTINT0"){
                varType=types->getInteger();
            }else if(t1->baseType=="KTRAT0"){
                varType=types->getRational();
            }else{
                EXCEPTION(true,setOperationType 18);
            }
        }else if(name=="KRD"
                 ||name=="KRDL"
                 ||name=="KWT"
                 ||name=="KWTL"
                     ){
            Type *t1 = ((Node *)childs->at(0))->varType;
            EXCEPTION(!t1,setOperationType 15);
            EXCEPTION(t1!=types->getInteger(),setOperationType 16);
            varType = t1;
        }else if(name=="KINT"
                     ){
            Type *t1 = ((Node *)childs->at(0))->varType;
            EXCEPTION(!t1,setOperationType 17);
            if(t1->baseType=="KTINT0"){
                varType=types->getInteger();
            }else if(t1->baseType=="KTRAT0"){
                varType=types->getInteger();
            }else{
               EXCEPTION(true,setOperationType 19);
            }
        }else if(name=="KNOT"
                     ){
            Type *t1 = ((Node *)childs->at(0))->varType;
            EXCEPTION(!t1,setOperationType 20);
            EXCEPTION(t1!=types->getInteger(),setOperationType 21);
            varType=types->getInteger();
        }else if(name=="KCDN"
                     ){
            Type *t1 = ((Node *)childs->at(0))->varType;
            Type *t2 = ((Node *)childs->at(1))->varType;
            EXCEPTION(!t1||!t2,setOperationType 22);
            EXCEPTION(t1!=types->getInteger(),setOperationType 23);
            EXCEPTION(t2!=types->getInteger(),setOperationType 24);
            varType = types->getRational();
        }
    }
}

bool Node::isCorrectType()
{
    return true;
}

void Node::processTypes(Types *types,Vars *vars)
{
    for (int i = 0; i < childs->size(); ++i) {
        Node *cur = childs->at(i);
        if(cur->name=="KTYP"){
            cur->processSectionType(types,vars);
        }
    }
}

void Node::processSectionType(Types *types,Vars *vars)
{
    for (int i = 0; i < childs->size(); ++i) {
        Node *cur = childs->at(i);
        if(cur->name=="OPINIT0"){//=
            QString typeName = ((Node *)cur->childs->at(0))->value;

            Type *t=NULL;
            Node *right=cur->childs->at(1);
            t = right->getTypeFromCurrentNode(types,vars,i,typeName);
            types->append(t);
        }
    }
}

void Node::addConstsForType(Type *t,Vars *vars){
    if(name=="ID"){
        Variable *v = new Variable(this);//cur->value
        v->setType(t);
        v->setIsConst(true);
        v->setValue(EnumType::counter++);
        vars->append(v);
    }
    for (int i = 0; i < childs->size(); ++i) {
        Node *cur = childs->at(i);
        cur->addConstsForType(t,vars);
    }
}

void Node::addArrayDiapazons(ArrayType *t){
    if(name=="OPDPS0"){//..
        Node *max = childs->at(0);
        Node *min = childs->at(1);
        t->addDimension(min,max);
    }
    for (int i = 0; i < childs->size(); ++i) {
        Node *cur = childs->at(i);
        cur->addArrayDiapazons(t);
    }
}

void Node::processVariables(Types *types, Vars *vars)
{
    for (int i = 0; i < childs->size(); ++i) {
        Node *cur = childs->at(i);
        if(cur->name=="KCON"){//const
            cur->processConstSection(types,vars);
        }else if(cur->name=="KVAR"){//var
            cur->processVarSection(types,vars);
        }
    }
}
void Node::processConstSection(Types *types, Vars *vars){
    for (int i = 0; i < childs->size(); ++i) {
        Node *cur = childs->at(i);
        if(cur->name=="OPINIT0"){//=
            Node *arg = cur->childs->at(1);
            if(arg->name=="CINT"){
                Variable *v = new Variable(cur->childs->at(0));//cur->value
                v->setType(types->getInteger());
                v->setIsConst(true);
                v->setValue(arg->value);
                vars->append(v);
            }else if(arg->name=="OPAR1"){//-
                Variable *v = new Variable(cur->childs->at(0));//cur->value
                v->setType(types->getInteger());
                v->setIsConst(true);
                EXCEPTION(((Node *)arg->childs->at(1))->name!="CINT",processConstSection);
                v->setValue(arg->value+((Node *)arg->childs->at(1))->value);
                vars->append(v);
            }else{
                qDebug()<<cur->toString()<<endl;
                EXCEPTION(true,processConstSection);
            }
        }
    }
}

void Node::processVarSection(Types *types, Vars *vars){
    for (int i = 0; i < childs->size(); ++i) {
        Node *cur = childs->at(i);
        if(cur->name=="SCOL0"){//:
            Variable *v = new Variable(cur->childs->at(0));
            vars->append(v);
            i++;
            Node *typeNode = childs->at(i);
            if(typeNode->childs->size()==0){
                Type *t = types->findTypeByName(typeNode->value);
                if(t==NULL){
                    qDebug()<<"undeclared type "<<typeNode->value<<endl;
                }
                EXCEPTION(t==NULL,processVarSection);
                v->setType(t);
            }else{
                Type *t = typeNode->getTypeFromCurrentNode(types,vars,i,types->getFreeName());
                types->append(t);
                v->setType(t);
            }
        }else if(cur->name=="ID"){
            Variable *v = new Variable(cur);
            vars->append(v);
            i++;
            Node *typeNode = childs->at(i);
            if(typeNode->name=="KLAB"){
                Type *t = types->findTypeByName("Label");
                EXCEPTION(t==NULL,processVarSection Label)
                v->setType(t);
            }else{
                EXCEPTION(true,processVarSection 1)
            }
        }
    }
}
Type *Node::getTypeFromCurrentNode(Types *types, Vars *vars,int &iterator,QString typeName){
    Type *t=NULL;
    if(     name=="KTINT0"||  //integer
            name=="KTRAT0"    //rational
            ){
        t = new SimpleType(typeName);
        t->setBaseType(this);
    }else if(name=="OPDPS0"){//  ..
        t = new RangeType(typeName);
        ((RangeType*)t)->setDiapazon(childs->at(1),childs->at(0));
    }else if(name=="SPO0"){//    (
        t = new EnumType(typeName);
        addConstsForType(t,vars);
    }else if(name=="SBKO0"){//   [
        t = new ArrayType(typeName);
        Node *node = NULL;
        if(parent->name!="OPINIT0"){
            node = parent->childs->at(iterator+2);
        }else{
            node = parent->parent->childs->at(iterator+2);
        }
        t->setBaseType(node);
        ((Node *)childs->at(1))->addArrayDiapazons((ArrayType*)t);
        iterator+=2;
    }else{
        qDebug()<<"to string = "<<toString()<<endl;
        EXCEPTION(true,getTypeFromCurrentNode);
    }
    return t;
}

bool Node::postProcessRecursive()
{

}

void Node::moveUp(Node *child)
{
    if(parent==NULL){
        qDebug()<<"Node::moveUp()::parent==NULL"<<endl;
        getwchar();
        exit(1);
    }
    this->removeNode(child);
    parent->insertChildAfter(this,child);

}

void Node::insertChildAfter(Node *arg,Node *child)
{
    child->parent=this;
    int childPos = getChildPosition(arg);
    if(childPos==-1){
        qDebug()<<"Node::insertChildAfter()::childPos==-1"<<endl;
        getwchar();
        exit(1);
    }
    childs->insert(childPos+1,child);
}
void Node::insertChildBefore(Node *arg, Node *child){
    child->parent=this;
    int childPos = getChildPosition(arg);
    if(childPos==-1){
        qDebug()<<"Node::insertChildBefore()::childPos==-1"<<endl;
        getwchar();
        exit(1);
    }
    childs->insert(childPos-1,child);
}

int Node::getChildPosition(Node *child)
{
    for (int i = 0; i < childs->size(); ++i) {
        Node * cur = childs->at(i);
        if(cur->id==child->id){
            return i;
        }
    }
    return -1;
}

Node *Node::getChild(QString name)
{
    for (int i = 0; i < childs->size(); ++i) {
        Node * cur = childs->at(i);
        if(cur->name==name){
            return cur;
        }
    }
    qDebug()<<"Node::getChild()::name="<<name<<endl;
    getwchar();
    exit(1);
    return NULL;
}

bool Node::hasChild(QString name)
{
    for (int i = 0; i < childs->size(); ++i) {
        Node * cur = childs->at(i);
        if(cur->name==name){
            return true;
        }
    }
    return false;
}
