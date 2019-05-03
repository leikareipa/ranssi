QT += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ranssi
TEMPLATE = app

OBJECTS_DIR = generated_files
RCC_DIR = generated_files
MOC_DIR = generated_files
UI_DIR = generated_files

SOURCES += src/main.cpp\
           src/gui/qt/windows/mainwindow.cpp

HEADERS += src/gui/qt/windows/mainwindow.h

FORMS += src/gui/qt/windows/ui/mainwindow.ui

# For GCC/Clang/MinGW.
QMAKE_CXXFLAGS += -g
QMAKE_CXXFLAGS += -O2
QMAKE_CXXFLAGS += -Wall
QMAKE_CXXFLAGS += -pipe
QMAKE_CXXFLAGS += -pedantic
QMAKE_CXXFLAGS += -std=c++11
