TEMPLATE = subdirs

INCLUDEPATH += $(BOOST_DIR) \
    ../third/cJSON/

HEADERS += \
    ../cxFWK/STPClient.h \
    ../cxFWK/STPServer.h \
    ../cxFWK/Util.h \
    ../cxFWK/WebSocketServer.h \
    ../cxFWK/Json.h

SUBDIRS += \
    eureka \
    example_json \
    example_service \
    example_ws_server


