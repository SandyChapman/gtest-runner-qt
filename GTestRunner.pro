TEMPLATE = app
TARGET = GTestRunner
QT += core \
    gui
HEADERS += include/GTestExecutable.h \
    include/GTestParser.h \
    include/GTestRunner.h
SOURCES += src/GTestExecutable.cpp \
    main.cpp \
    src/GTestParser.cpp \
    src/GTestRunner.cpp
FORMS += 
RESOURCES += 
INCLUDEPATH += include
