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

CXFWK_DIR = ../..

INCLUDEPATH += $(BOOST_DIR) \
                $${CXFWK_DIR}/cxFWK/ \
                $${CXFWK_DIR}/third/cJSON/

SOURCES += \
        $${CXFWK_DIR}/third/cJSON/cJSON.c \
        $${CXFWK_DIR}/program/eureka.cpp


