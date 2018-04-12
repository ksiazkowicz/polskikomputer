#-------------------------------------------------
#
# Project created by QtCreator 2016-03-22T09:10:26
#
#-------------------------------------------------

QT       += core gui script

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
RC_ICONS = gfx/icon.ico

TARGET = Polski_Komputer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
        canvas.cpp \
        GameCanvas.cpp \
    player.cpp \
    game.cpp \
    pointer.cpp \
    progress.cpp \
    minigamedialog.cpp \
    enddialog.cpp

HEADERS  += mainwindow.h \
            canvas.h \
            GameCanvas.h \
    player.h \
    game.h \
    pointer.h \
    progress.h \
    minigamedialog.h \
    enddialog.h

FORMS    += mainwindow.ui \
    minigamedialog.ui \
    enddialog.ui

RESOURCES += \
    game_assets.qrc

DISTFILES +=
