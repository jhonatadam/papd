QT += sql
TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle

LIBS += -L/opt/ibm/ILOG/CPLEX_Studio1261/cplex/lib/x86-64_linux/static_pic/
LIBS += -L/opt/ibm/ILOG/CPLEX_Studio1261/concert/lib/x86-64_linux/static_pic/

INCLUDEPATH += /opt/ibm/ILOG/CPLEX_Studio1261/cplex/include/
INCLUDEPATH += /opt/ibm/ILOG/CPLEX_Studio1261/concert/include/

DEFINES += IL_STD

LIBS +=  -lilocplex -lconcert -lcplex -lm -lpthread

SOURCES += main.cpp \
    datainterface.cpp \
    data.cpp \
    vars.cpp \
    exporter.cpp

HEADERS += \
    datainterface.h \
    data.h \
    vars.h \
    exporter.h

DISTFILES += \
    modelo.lp \
    modelo.pdf


