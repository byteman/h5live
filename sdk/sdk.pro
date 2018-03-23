#-------------------------------------------------
#
# Project created by QtCreator 2018-03-20T14:54:19
#
#-------------------------------------------------

QT       -= core gui

TARGET = sdk
TEMPLATE = lib

DEFINES += SDK_LIBRARY

INCLUDEPATH+=../ext/include
INCLUDEPATH+=../ext/libflv/include

LIBS+=-L../ext/lib -lPocoFoundationd


SOURCES += \
    ../src/CWHttpServer.cpp \
    ../src/CWRequestHandler.cpp \
    ../src/CWRequestHandlerFactory.cpp \
    ../src/CWWebSocketServer.cpp \
    ../src/dataqueue.cpp \
    ../src/flvmuxer.cpp \
    ../src/h5liveserver.cpp \
    ../src/mediatype.cpp \
    ../src/MediaTypeMapper.cpp \
    ../src/streamchannel.cpp \
    ../ext/libflv/src/BaseWriter.cpp \
    ../ext/libflv/src/FlvEncoder.cpp \
    ../ext/libflv/src/flv.c \
    ../ext/libflv/src/flv_bytestream.c

HEADERS +=\
    ../src/common.h \
    ../src/CWHttpServer.h \
    ../src/CWLogger.h \
    ../src/CWRequestHandler.h \
    ../src/CWRequestHandlerFactory.h \
    ../src/CWWebSocketServer.h \
    ../src/datapacket.h \
    ../src/dataqueue.h \
    ../src/flvmuxer.h \
    ../src/h5liveserver.h \
    ../src/mediatype.h \
    ../src/MediaTypeMapper.h \
    ../ext/libflv/src/flv_bytestream.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
