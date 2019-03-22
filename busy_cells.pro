#-------------------------------------------------
#
# Project created by QtCreator 2018-06-23T18:36:28
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 warn_off
CONFIG += static
TARGET = busy_cells
TEMPLATE = app
CONFIG += optimize_full
QMAKE_LFLAGS += -static -static-libgcc
QMAKE_LFLAGS_RELEASE += -static -static-libgcc

SOURCES += \
    entities/cell.cpp \
    entities/joint.cpp \
    busy_cells.cpp \
    draw.cpp \
    world.cpp \
    physics.cpp \
    world2.cpp \
    nn.cpp \
    quads.cpp

HEADERS += \
    entities/cell.h \
    entities/joint.h \
    mainwindow.h \
    vect.h \
    world.h \
    test.h \
    physics.h \
    world2.h \
    nn.h \
    quads.h \
    hsvtorgb.h

FORMS += \
    mainwindow.ui
