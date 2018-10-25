QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = speed-reader
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
	lib/wsm/wsm.cpp \
	src/main.cpp \
	src/main-window.cpp

HEADERS += \
	lib/wsm/wsm.h \
	lib/q-str-exception.h \
	src/main-window.h

FORMS += \
	form/main-window.ui

CONFIG += c++14
QMAKE_CXXFLAGS += -Wall -Wextra -pedantic
win32:RC_ICONS += icon/icon.ico
UI_DIR = src

QT += serialport

VERSION_MAJOR = 1
VERSION_MINOR = 1

DEFINES += "VERSION_MAJOR=$$VERSION_MAJOR"\
       "VERSION_MINOR=$$VERSION_MINOR"

#Target version
VERSION = $${VERSION_MAJOR}.$${VERSION_MINOR}

RESOURCES += speed-reader.qrc
