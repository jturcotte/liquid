TEMPLATE = app
TARGET = liquid
DESTDIR = $$OUT_PWD/..

QT += declarative webkit network sql xml

# Input
SOURCES += \
    backend.cpp \
    browserapplication.cpp \
    browserwindow.cpp \
    cookiejar.cpp \
    dwmhelper.cpp \
    historydatabase.cpp \
    historylocator.cpp \
    historyitem.cpp \
    mousewheelarea.cpp \
    qdeclarativewebview.cpp \
    searchlocator.cpp \
    singleapplication.cpp \
    tab.cpp \
    tabmanager.cpp \
    main.cpp \

HEADERS += \
    backend.h \
    browserapplication.h \
    browserwindow.h \
    cookiejar.h \
    dwmhelper.h \
    historydatabase.h \
    historylocator.h \
    historyitem.h \
    location.h \
    mousewheelarea.h \
    qdeclarativewebview_p.h \
    qobjectlistmodel.h \
    searchlocator.h \
    singleapplication.h \
    tab.h \
    tabmanager.h \

