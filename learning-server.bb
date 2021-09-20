inherit autotools
inherit wave-license

DESCRIPTION = "Learning-Server service's bin"
SECTION = "learning-server"

PR = "r0"

FILESPATH := "${WWWWAAAAAVVVVVEEEEEE_SRC_DIR}/learning-server:"
SRC_URI = "file://bin/"

S = "${WORKDIR}/bin/"

DEPENDS = "servicelayer openssl dlt-daemon tcm gtest boost"
