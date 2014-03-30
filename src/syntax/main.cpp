#include <QApplication>
#include "mainwindow.h"
//#include <QCoreApplication>
#include "xmlreader.h"
#include "lex_analyzer.h"
#include "syn_analyzer.h"

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    MainWindow w;


    QTreeView *treeView = new QTreeView(&w);
         w.setCentralWidget(treeView);
         QStandardItemModel *standardModel = new QStandardItemModel ;
         QStandardItem *rootNode = standardModel->invisibleRootItem();

    std::cout<< "Hello\n";
    QMap<QString, TokenType*> *keywords = new QMap<QString, TokenType*>;
    QMap<QString, TokenType*> *types = new QMap<QString, TokenType*>;
    QMap<QString, TokenType*> *operators = new QMap<QString, TokenType*>;
    QMap<QString, TokenType*> *ids = new QMap<QString, TokenType*>;
    QMap<QString, TokenType*> *constants = new QMap<QString, TokenType*>;
    XMLReader *XMLR = new XMLReader(keywords, types, operators, ids, constants);
    XMLR->parse_XML("SRCTables.xml");
    Lex_analyzer * lex = new Lex_analyzer(keywords, types, operators, ids, constants);
    XMLR->writeTokensXML("Tokens-res.xml", lex->analyze(argv[1]));
    XMLR->writeTablesXML("SRCTables-res.xml");

    QList<Grammar*> *grammars = new QList<Grammar*>();
    XMLR->setGrammars(grammars);
    qDebug() << "starting syntax analysis" <<endl;
    QString fileGrammar = "Grammars.xml";
    XMLR->readGrammars(&fileGrammar);
    QListIterator<Grammar*> i(*grammars);
    //i.next()->print();
    qDebug()<<"Checking grammars..." <<endl;
    while(i.hasNext()){
        //qDebug() <<endl;
        //i.next()->print();
        //qDebug() <<endl;
        Grammar* gr = i.next();
        gr->checkGrammar();
        gr->generateFIRST_SET_with_eps();
        gr->generateTable();
    }
    qDebug()<<"All grammars checked." <<endl;
    getwchar();
    grammars->at(0)->generateFIRST_SET_with_eps();
    grammars->at(6)->generateFIRST_SET_with_eps();
    grammars->at(6)->generateTable();
    /*QMap<QString, QList<QString>*>::iterator k;
    for(k = grammars->at(6)->getFIRST()->begin(); k!=grammars->at(6)->getFIRST()->end(); k++){
        QString nonter = k.key();
        qDebug() <<"First set for" <<nonter <<": ";
        QListIterator<QString> j(*k.value());
        while(j.hasNext()){
            qDebug() <<j.next();
        }
        qDebug() <<endl;
    }*/
    /*QPair<QString, QString> check("COMO","KBG");
    QPair<QList<QString>*, int>* res = grammars->at(6)->getTable()->value(check);
    qDebug() <<res->first->at(2);
    qDebug() <<res->second;
    qDebug() <<"size: "<<grammars->at(6)->getTable()->size();*/
    qDebug()<<"Starting syntax:" <<endl;
    Syn_analyzer *syn = new Syn_analyzer(lex->getTokens(), grammars, rootNode);
    syn->analyze();
    delete (syn);
    delete (keywords);
    delete (types);
    delete (operators);
    delete (ids);
    delete (constants);
    delete (lex);
    delete (grammars);
    std::cout<< "\n finished\n\n";


    /*QGraphicsScene scene(QRectF(0,0,1000,1000));
    QGraphicsView view(&scene);
    view.resize(QSize(1000,1000));
    QGraphicsRectItem* pRectItem = new QGraphicsRectItem(0, &scene);
    pRectItem->setPen(QPen(Qt::black));
    pRectItem->setBrush(QBrush(Qt::green));
    pRectItem->setRect(QRectF(-30,-30,120,80));
    pRectItem->setFlags(QGraphicsItem::ItemIsMovable);

    //QGraphicsPixmapItem* = scene.addPixmap(QPixmap(Qt::black));
    QGraphicsTextItem* pTextItem = scene.addText("Move it!");
    pTextItem->setFlags(QGraphicsItem::ItemIsMovable);

    QGraphicsLineItem* pLineItem= scene.addLine(QLineF(0, 0, 100, 50), QPen(Qt::red,2));
    pLineItem->setFlags(QGraphicsItem::ItemIsMovable);

    view.show();
*/


         //register the model
         treeView->setModel(standardModel);
         treeView->expandAll();

         //selection changes shall trigger a slot
         //QItemSelectionModel *selectionModel= treeView->selectionModel();
         //QObject::connect(selectionModel, SIGNAL(selectionChanged (const QItemSelection &, const QItemSelection &)));
w.show();


    return a.exec();
}
