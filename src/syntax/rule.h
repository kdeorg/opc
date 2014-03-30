#ifndef RULE_H
#define RULE_H
#include <QString>
#include <QList>
#include <QDebug>
class Rule
{
private:
    QString left;
    QList<QString>* right;
    int id;
public:
    Rule(int id_, QString* left, QList<QString>* right);
    ~Rule();

    QString getLeft();
    QList<QString>* getRight();

    void print();

    int getId();
};

#endif // RULE_H
