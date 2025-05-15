QT += widgets
CONFIG += c++17 console
TEMPLATE = app

SOURCES += main.cpp \
           MainWindow.cpp \
           MatrixInputWidget.cpp \
           Parser.cpp \
           Solver.cpp

HEADERS += MainWindow.h \
           MatrixInputWidget.h \
           Parser.h \
           Solver.h

INCLUDEPATH += ./boost/boost_1_88_0
LIBS += -lgmp -lmpfr
