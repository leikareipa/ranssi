QT += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ranssi
TEMPLATE = app

OBJECTS_DIR = generated_files
RCC_DIR = generated_files
MOC_DIR = generated_files
UI_DIR = generated_files

SOURCES += src/main.cpp\
           src/gui/windows/mainwindow.cpp \
           src/gui/widgets/QTextEdit_text_editor.cpp \
           src/gui/widgets/QWidget_text_editor_overlay.cpp \
           src/wav/wav.cpp \
           src/wav/playback.cpp \
           src/project/project.cpp \
           src/gui/widgets/QWidget_wav_player.cpp \
           src/text/elements.cpp \
           src/text/formatting.cpp \
           src/text/syntax.cpp \
           src/gui/widgets/QWidget_tarpaulin.cpp \
           src/wav/conversion.cpp

HEADERS += src/gui/windows/mainwindow.h \
           src/gui/widgets/QTextEdit_text_editor.h \
           src/gui/widgets/QWidget_text_editor_overlay.h \
           src/wav/wav.h \
           src/common.h \
           src/wav/playback.h \
           src/project/project.h \
           src/gui/widgets/QWidget_wav_player.h \
           src/text/elements.h \
           src/text/formatting.h \
           src/text/syntax.h \
           src/text/metadata.h \
           src/gui/widgets/QWidget_tarpaulin.h \
           src/wav/conversion.h \
           src/csv.h \
           src/gui/utility.h

FORMS += src/gui/windows/ui/mainwindow.ui

INCLUDEPATH += $$PWD/src/gui/widgets \
               $$PWD/src

# For GCC/Clang/MinGW.
QMAKE_CXXFLAGS += -g
QMAKE_CXXFLAGS += -O2
QMAKE_CXXFLAGS += -Wall
QMAKE_CXXFLAGS += -pipe
QMAKE_CXXFLAGS += -pedantic
QMAKE_CXXFLAGS += -std=c++11
