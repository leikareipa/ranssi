QT += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ranssi
TEMPLATE = app

OBJECTS_DIR = generated_files
RCC_DIR = generated_files
MOC_DIR = generated_files
UI_DIR = generated_files

SOURCES += src/main.cpp\
           src/gui/qt/windows/mainwindow.cpp \
           src/gui/qt/subclasses/QTextEdit_text_field.cpp

HEADERS += src/gui/qt/windows/mainwindow.h \
           src/gui/qt/subclasses/QTextEdit_text_field.h

FORMS += src/gui/qt/windows/ui/mainwindow.ui

INCLUDEPATH += $$PWD/src/gui/qt/subclasses

# For GCC/Clang/MinGW.
QMAKE_CXXFLAGS += -g
QMAKE_CXXFLAGS += -O2
QMAKE_CXXFLAGS += -Wall
QMAKE_CXXFLAGS += -pipe
QMAKE_CXXFLAGS += -pedantic
QMAKE_CXXFLAGS += -std=c++11
