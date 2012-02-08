TEMPLATE = app
TARGET = liquid
DESTDIR = $$OUT_PWD/..

QT += declarative network
SOURCES = \
    browserapplication.cpp \
    browserwindow.cpp \
    dwmhelper.cpp \
    singleapplication.cpp \
    main.cpp \

HEADERS = \
    browserapplication.h \
    browserwindow.h \
    dwmhelper.h \
    singleapplication.h \

