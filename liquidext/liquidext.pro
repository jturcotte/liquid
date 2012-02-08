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
    qobjectlistmodel.h \
    searchlocator.h \
    tab.h \
    tabmanager.h

WK2_WP_PATH = $$system(which QtWebProcess)
!isEmpty(WK2_WP_PATH) {
    # Temporary cheap logic:
    # If we find QtWebProcess in PATH, assume that this is a WK2 build.
    WK_BUILD_PATH = $$replace(WK2_WP_PATH, "/bin/QtWebProcess", "")
    # Extract the source path from the top of the Makefile.
    WK_SOURCES_PATH = $$WK_BUILD_PATH/$$system("sed -rne 's/$$LITERAL_HASH Project:\\s*(.*)\\/WebKit.pro/\\1/p' $$WK_BUILD_PATH/Makefile")
    QT -= webkit
    # FIXME: Having to add the source paths to includes is quite annoying.
    INCLUDEPATH += $$WK_BUILD_PATH/include $$WK_BUILD_PATH/include/WebKit2 $$WK_BUILD_PATH/include/QtWebKit $$WK_SOURCES_PATH $$WK_SOURCES_PATH/WebKit2
    LIBS += -lQtWebKit -L$$WK_BUILD_PATH/lib
    QMAKE_RPATHDIR = $$WK_BUILD_PATH/lib $$QMAKE_RPATHDIR

    DEFINES += WK2_BUILD=1
    SOURCES += qdeclarativewkview.cpp
    HEADERS += qdeclarativewkview_p.h
} else {
    SOURCES += qdeclarativewebview.cpp
    HEADERS += qdeclarativewebview_p.h
}

copy_qmldir.target = $$OUT_PWD/qmldir
# Use the Makefile variable directly to get the final output, qmake doesn't know about it yet here.
copy_qmldir.depends = $(OBJECTS)
copy_qmldir.commands = echo plugin $$TARGET  > \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
QMAKE_EXTRA_TARGETS += copy_qmldir
POST_TARGETDEPS += $$copy_qmldir.target

