TARGET=tst_qservicecontext
INCLUDEPATH += ../../../serviceframework

CONFIG+=testcase

QT = core

include(../../../common.pri)

# Input 
HEADERS += 
SOURCES += tst_qservicecontext.cpp 

qtAddLibrary(QtServiceFramework)

symbian {
    TARGET.CAPABILITY = ALL -TCB
}