HEADERS += src/mainwindow.h \
           painterwidget.h \
           qglwindow.h \
           qlglwindow.h \
           src/qoglwidget.h \
           src/arbeditor.h \
           src/qglarbwindow.h \
           src/glsleditor.h \
           src/qglglslwindow.h \
           src/codeeditor.h

SOURCES += main.cpp \
           src/mainwindow.cpp \
           painterwidget.cpp \
           qglwindow.cpp \
           qlglwindow.cpp \
           src/arbeditor.cpp \
           src/qglarbwindow.cpp \
           src/glsleditor.cpp \
           src/qglglslwindow.cpp \
           src/codeeditor.cpp

HEADERS += $$files(headers/*.h)

HEADERS += $$files(glvertex/*.h)

HEADERS += $$files(gfx/*.h)
SOURCES += $$files(gfx/*.cpp)

HEADERS += $$files(pvm/*.h)
SOURCES += $$files(pvm/*.cpp)

HEADERS += $$files(dicom/*.h)
SOURCES += $$files(dicom/*.cpp)
