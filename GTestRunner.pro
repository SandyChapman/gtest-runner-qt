TEMPLATE = app

CONFIG += debug_and_release
CONFIG(debug, debug|release) {
	TARGET = gtestrunner-debug
} 
else {
	TARGET = gtestrunner
}
QT += core gui xml
INCLUDEPATH += include
HEADERS += include/GTestSuiteResults.h \
    include/GTestResults.h \
    include/GTestSuite.h \
    include/Defines.h \
    include/GTestExecutableResults.h \
    include/GTest.h \
    include/GTestExecutable.h \
    include/GTestParser.h \
    include/GTestRunner.h \
    include/TestTreeModel.h \
    include/TestTreeItem.h 
SOURCES += src/GTestResults.cpp \
    src/GTestSuiteResults.cpp \
    src/GTestSuite.cpp \
    src/GTestExecutableResults.cpp \
    src/GTest.cpp \
    src/GTestExecutable.cpp \
    src/main.cpp \
    src/GTestParser.cpp \
    src/GTestRunner.cpp \
    src/TestTreeModel.cpp \
    src/TestTreeItem.cpp 
FORMS += resources/gtestrunner.ui
UI_HEADERS_DIR = include
UI_SOURCES_DIR = src
RESOURCES += resources/gtestrunner.qrc
