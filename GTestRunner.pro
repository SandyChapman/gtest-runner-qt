TEMPLATE = app
TARGET = GTestRunner
QT += core \
    gui
HEADERS += include/GTest.h \
    include/GTestFixture.h \
    include/GTestExecutable.h \
    include/GTestParser.h \
    include/GTestRunner.h
SOURCES += src/GTestFixture.cpp \
    src/GTest.cpp \
    src/GTestExecutable.cpp \
    main.cpp \
    src/GTestParser.cpp \
    src/GTestRunner.cpp
FORMS += 
RESOURCES += 
INCLUDEPATH += include
