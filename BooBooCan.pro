QT += core gui widgets sql svg

CONFIG += c++17
TEMPLATE = app
TARGET = BooBooCan

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    databasemanager.cpp \
    readydrinkswidget.cpp \
    settingswidget.cpp \
    customdrinkwidget.cpp \
    orderstatuswidget.cpp

HEADERS += \
    mainwindow.h \
    databasemanager.h \
    readydrinkswidget.h \
    drink.h \
    order.h \
    settingswidget.h \
    customdrinkwidget.h \
    orderstatuswidget.h
