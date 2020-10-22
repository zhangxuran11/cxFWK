TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

CXFWK_DIR = ../..

INCLUDEPATH += $(BOOST_DIR) \
                $${CXFWK_DIR}/cxFWK/ \
                $${CXFWK_DIR}/third/cJSON/

SOURCES += \
        $${CXFWK_DIR}/example/example_json.cpp \
        $${CXFWK_DIR}/third/cJSON/cJSON.c
