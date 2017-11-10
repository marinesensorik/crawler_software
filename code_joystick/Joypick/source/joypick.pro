# -------------------------------------------------
# Project created by QtCreator 2011-01-30T10:26:20
# -------------------------------------------------
QT += network
TARGET = Joypick
TEMPLATE = app
SOURCES += main.cpp \
    joypick.cpp \
    qjoystick.cpp
HEADERS += joypick.h \
    qjoystick.h
LIBS += -lSDL
win32 {
DEFINES += SDL_WIN
}
