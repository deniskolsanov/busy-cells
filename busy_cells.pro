#-------------------------------------------------
#
# Project created by QtCreator 2018-06-23T18:36:28
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11 warn_off
TARGET = busy_cells
TEMPLATE = app
QMAKE_CXXFLAGS += -O3

SOURCES += \
    entities/cell.cpp \
    entities/creature.cpp \
    entities/joint.cpp \
    entities/virus.cpp \
    busy_cells.cpp \
    draw.cpp \
    world.cpp

HEADERS += \
    entities/cell.h \
    entities/creature.h \
    entities/joint.h \
    entities/virus.h \
    mainwindow.h \
    vect.h \
    world.h

FORMS += \
    mainwindow.ui
