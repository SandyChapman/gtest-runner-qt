TEMPLATE = app
TARGET = GTestRunner
QT += core \
    gui
HEADERS += include/GTestSuite.h \
    include/Defines.h \
    include/GTestExecutableResults.h \
    include/GTest.h \
    include/GTestExecutable.h \
    include/GTestParser.h \
    include/GTestRunner.h
SOURCES += src/GTestSuite.cpp \
    src/GTestExecutableResults.cpp \
    src/GTest.cpp \
    src/GTestExecutable.cpp \
    main.cpp \
    src/GTestParser.cpp \
    src/GTestRunner.cpp
FORMS += 
RESOURCES += 
INCLUDEPATH += include
