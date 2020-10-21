TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

DEFINES += BOOST_BIND_GLOBAL_PLACEHOLDERS

win32 {
    LIBS += -lwsock32 -lws2_32
}

unix {

}

INCLUDEPATH += $(BOOST_INC) \
                ../inc/

SOURCES += \
        ../program/eureka.cpp

HEADERS += \
    ../inc/STPClient.h \
    ../inc/STPServer.h \
    ../inc/Util.h \
    ../inc/WebSocketServer.h
