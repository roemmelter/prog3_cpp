TARGET = myqtapp
TEMPLATE = app

include(vis-framework.pri)

INCLUDEPATH += src/
INCLUDEPATH += headers/
INCLUDEPATH += glvertex/

QT += core opengl

message(QT_VERSION = $$QT_VERSION)
greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
    DEFINES += HAVE_QT5
}
greaterThan(QT_MAJOR_VERSION, 5) {
    QT += openglwidgets core5compat
    DEFINES += HAVE_QT6
}

defineTest(iconv) {
    iconv_header = /usr/local/include/iconv.h
    !exists($$iconv_header) {
            return(false)
    }
    return(true)
}

defineTest(dcmtk) {
    dcmtk_header = /usr/local/include/dcmtk/dcmdata/dctk.h
    !exists($$dcmtk_header) {
            return(false)
    }
    return(true)
}

unix:dcmtk() {
    message(DCMTK found)
    DEFINES += HAVE_CONFIG_H
    DEFINES += HAVE_DCMTK
    INCLUDEPATH += /usr/local/include
    QMAKE_LIBDIR += /usr/local/lib
    LIBS += -ldcmdata -ldcmjpeg -lijg8 -lijg12 -lijg16 -ldcmtls -loflog -lofstd -lz
    unix:iconv(): LIBS += -liconv
}

MOC_DIR = mocs
OBJECTS_DIR = objs
RCC_DIR = rccs
UI_DIR = uics

INCLUDEPATH += headers/
!mac: CONFIG += c++11
mac: CONFIG -= app_bundle

unix:QMAKE_CXXFLAGS_RELEASE = -Os
unix:QMAKE_CXXFLAGS_WARN_ON += -Wall -Wno-unused-parameter -Wno-parentheses -Wno-deprecated-declarations -Wno-unknown-pragmas

unix:!mac:LIBS += -lGL -lGLU

DESTDIR = $$PWD
