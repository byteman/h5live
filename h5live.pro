TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH+=ext/include
INCLUDEPATH+=ext/ffmpeg/include
INCLUDEPATH+=ext/opencv310/opencv310/include
INCLUDEPATH+=ext/libflv/include
LIBS+=-L./ext/lib -lPocoFoundationd
LIBS+=-L./ext/ffmpeg/lib -lavcodec -lavformat -lavutil -lswscale
LIBS+=-L./ext/opencv310/lib32/ -lopencv_world310
SOURCES += src/main.cpp \
    src/ffmpegencoder.cpp \
    src/videopp.cpp \
    src/ffutils.cpp \
    src/videoqueue.cpp \
    src/pusherpair.cpp \
    src/rtsppusher.cpp \
    src/pushermanager.cpp \
    src/dataqueue.cpp \
    src/flvmuxer.cpp \
    src/streamchannel.cpp \
    src/CWWebSocketServer.cpp \
    ext/libflv/src/BaseWriter.cpp \
    ext/libflv/src/FlvEncoder.cpp \
    ext/libflv/src/flv_bytestream.c \
    src/CWHttpServer.cpp \
    src/CWRequestHandlerFactory.cpp \
    src/CWRequestHandler.cpp \
    src/mediatype.cpp \
    src/MediaTypeMapper.cpp \
    src/h5liveserver.cpp \
    src/h5liveserver.cpp

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    src/CWLogger.h \
    src/ffmpegencoder.h \
    src/videopp.h \
    src/ffutils.h \
    src/videoqueue.h \
    src/pushermanager.h \
    src/pusherpair.h \
    src/rtsppusher.h \
    src/flvwriter.h \
    src/dataqueue.h \
    src/flvmuxer.h \
    src/datapacket.h \
    src/streamchannel.h \
    src/CWWebSocketServer.h \
    ext/libflv/src/flv_bytestream.h \
    src/CWHttpServer.h \
    src/CWRequestHandlerFactory.h \
    src/CWRequestHandler.h \
    src/mediatype.h \
    src/MediaTypeMapper.h \
    src/h5liveserver.h \
    src/h5liveserver.h

