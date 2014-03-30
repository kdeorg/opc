#-------------------------------------------------
#
# Project created by QtCreator 2014-03-28T05:58:34
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SynTree
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    lex_analyzer.cpp \
    syn_analyzer.cpp \
    grammar.cpp \
    rule.cpp \
    xmlreader.cpp

HEADERS  += mainwindow.h \
    lex_analyzer.h \
    syn_analyzer.h \
    grammar.h \
    rule.h \
    xmlreader.h \
    defines.h

#FORMS    += mainwindow.ui

OTHER_FILES += \
    Grammars.xml \
    SRCTables.xml
