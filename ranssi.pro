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
           src/gui/qt/widgets/QTextEdit_text_editor.cpp \
           src/gui/qt/widgets/QWidget_text_editor_overlay.cpp \
           src/wav/wav.cpp \
           src/gui/qt/widgets/QWidget_wav_display.cpp

HEADERS += src/gui/qt/windows/mainwindow.h \
           src/gui/qt/widgets/QTextEdit_text_editor.h \
           src/gui/qt/widgets/QWidget_text_editor_overlay.h \
           src/wav/wav.h \
           src/common.h \
           src/gui/qt/widgets/QWidget_wav_display.h

FORMS += src/gui/qt/windows/ui/mainwindow.ui

INCLUDEPATH += $$PWD/src/gui/qt/widgets \
               $$PWD/src

# For GCC/Clang/MinGW.
QMAKE_CXXFLAGS += -g
QMAKE_CXXFLAGS += -O2
QMAKE_CXXFLAGS += -Wall
QMAKE_CXXFLAGS += -pipe
QMAKE_CXXFLAGS += -pedantic
QMAKE_CXXFLAGS += -std=c++11
