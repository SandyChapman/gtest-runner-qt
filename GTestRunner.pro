TEMPLATE = app
TARGET = GTestRunner
QT += core \
    gui
HEADERS += include/GTest.h \
    include/GTestParser.h \
    include/GTestRunner.h
SOURCES += main.cpp \
    src/GTest.cpp \
    src/GTestParser.cpp \
    src/GTestRunner.cpp
FORMS += 
RESOURCES += 
INCLUDEPATH += include
