# Makefile.am - make executable for service layer process
bin_PROGRAMS = RTMBackend

RTMBackend_SOURCES := \
    server.cpp \
    Protocol.cpp \
    Socket.cpp \
    SSLSocket.cpp


RTMBackend_CPPFLAGS := \
    -I$(stincdir)/servicelayer \
    -I$(top_srcdir) \
    -O2 \
    -g \
    -Wall \
    -Wno-unused-parameter \
    -std=c++14 \
    $(LGE_BIN_CFLAGS) \
    -D_STDATOMIC_HAVE_ATOMIC \
    -D_MORE_MARSHM \
    -DBMW_WAVE
#       -DFOR_TESTING \
#       -DREAD_CERT_FROM_FILE \
#    -fpermissive \
#    -I$(stincdir)/arch/linux-arm \
#    -I$(workspace)/servicelayer/core/service_layer/include \
#    -D_FORTIFY_SOURCE=2

RTMBackend_LDFLAGS := \
    -lwatchdog_client \
    -lcorebase \
    -lserviceutils \
    -lutils \
    -lbinder \
    -llog \
    -ldlt \
    -lssl \
    -lcrypto \
    -lrt \
    -pthread \
    -ltcm \
    -lboost_system \
    -lboost_filesystem \
    -lboost_thread \
    $(LGE_BIN_LDFLAGS)
#    -lisystem \
#    -z,relro \
#    -z,now \
#    -pie
