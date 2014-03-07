#-------------------------------------------------
#
# Project created by QtCreator 2014-03-05T11:52:47
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = LEX
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    xmlreader.cpp \
    lex_analyzer.cpp

OTHER_FILES += \
    SRCTables.xml

HEADERS += \
    xmlreader.h \
    lex_analyzer.h \
    defines.h
