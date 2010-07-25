TEMPLATE = app
TARGET = GTestRunner
QT += core \
    gui
HEADERS += include/GTest.h \
    include/GTestParser.h \
    include/GTestRunner.h
SOURCES += src/GTest.cpp \
    src/GTestParser.cpp \
    src/GTestRunner.cpp \
    main.cpp
FORMS += 
RESOURCES += 
INCLUDEPATH += include
