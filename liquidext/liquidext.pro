TARGET  = liquid

TEMPLATE = lib
QT += declarative webkit network sql xml
CONFIG += qt plugin
# Prevent the debug/release destdir for Windows builds.
DESTDIR = $$OUT_PWD

TARGET = $$qtLibraryTarget($$TARGET)


# Input
SOURCES += \
    backend.cpp \
    cookiejar.cpp \
    historydatabase.cpp \
    historylocator.cpp \
    historyitem.cpp \
    mousewheelarea.cpp \
    qdeclarativewebview.cpp \
    searchlocator.cpp \
    plugin.cpp \
    tab.cpp \
    tabmanager.cpp
HEADERS += \
    backend.h \
    cookiejar.h \
    historydatabase.h \
    historylocator.h \
    historyitem.h \
    location.h \
    mousewheelarea.h \
    qdeclarativewebview_p.h \
    qobjectlistmodel.h \
    searchlocator.h \
    tab.h \
    tabmanager.h

copy_qmldir.target = $$OUT_PWD/qmldir
# Use the Makefile variable directly to get the final output, qmake doesn't know about it yet here.
copy_qmldir.depends = $(OBJECTS)
copy_qmldir.commands = echo plugin $$TARGET  > \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
QMAKE_EXTRA_TARGETS += copy_qmldir
POST_TARGETDEPS += $$copy_qmldir.target

