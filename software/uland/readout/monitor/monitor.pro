# -------------------------------------------------
# Project created by QtCreator 2011-06-01T19:14:12
# -------------------------------------------------
QT += network
TARGET = monitor
TEMPLATE = app
SOURCES += main.cpp \
    monitor.cpp \
    hw_access.cpp \
    console.cpp \
    rateplot.cpp
HEADERS += monitor.h \
    libaltix.h \
    hw_access.h \
    console.h \
    altix_userland.h \
    rateplot.h
FORMS += monitor.ui \
    console.ui
INCLUDEPATH += /usr/include/qwt-qt4/
LIBS += -Wl,-rpath,.,-laltix \
    -L$$PWD \
    -laltix \
    -lqwt-qt4 \
    -O3
libaltix.target = make_lib
libaltix.commands = cd \
    ../../libaltix; \
    $(MAKE) \
    lib; \
    cp \
    libaltix.so \
    ../readout/monitor/; \
    cp \
    libaltix.h \
    ../readout/monitor/; \
    cp \
    altix_userland.h \
    ../readout/monitor; \
    $(MAKE) \
    clean;
libaltix.depends = 
QMAKE_EXTRA_UNIX_TARGETS += libaltix
RESOURCES += monitor_res.qrc
