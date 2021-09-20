TEMPLATE = lib
CONFIG += c++11
CONFIG -= qt

INCLUDEPATH += \
        D:/hunght/Wave_TPM4G/3.SourceCode/IRTM/include \
        D:/hunght/Wave_TPM4G/3.SourceCode/IRTM/include/servicelayer \
        D:/hunght/Wave_TPM4G/3.SourceCode/IRTM/include/CommonAPI-3.1 \
        D:/hunght/Wave_TPM4G/3.SourceCode/IRTM/include/utils \
        D:/hunght/Wave_TPM4G/3.SourceCode/IRTM/include/tcm \
        D:/hunght/Wave_TPM4G/3.SourceCode/IRTM/include/services/TelephonyManagerService/base \
        D:/hunght/Wave_TPM4G/3.SourceCode/IRTM/include/servicelayer/core/service_layer/include/services/ApplicationManagerService

SUBDIRS += \
    rtm-server.pro

HEADERS += \
    ICommHandler.h \
	ISocket.h \
	Protocol.h \
	server.h \
	Socket.h \
	SSLSocket.h
	

SOURCES += \
    client.cpp \
	client_tls.cpp \
	Protocol.cpp \
	server.cpp \
	Socket.cpp \
        SSLSocket.cpp

DISTFILES += \
    Makefile.am \
    configure.ac \
        install.bat \
	build_server.sh

