inherit autotools
inherit wave-license

DESCRIPTION = "RTM-Server service's bin"
SECTION = "rtm-server"

PR = "r0"

FILESPATH := "${WWWWAAAAAVVVVVEEEEEE_SRC_DIR}/rtm-server:"
SRC_URI = "file://bin/"

S = "${WORKDIR}/bin/"

DEPENDS = "servicelayer openssl dlt-daemon tcm gtest boost"
