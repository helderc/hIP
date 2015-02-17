
QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = hIP
TEMPLATE = app

win32 {
    INCLUDEPATH += C:/OpenCV246/include/
    LIBS += -LC:/OpenCV246/lib/ -llibopencv_core246 -llibopencv_highgui246 -llibopencv_imgproc246
}

unix {
    LIBS += -lopencv_core -lopencv_highgui -lopencv_imgproc
}

HEADERS += mainwindow.h \
    LabelImage.h \
    EdgeDetectShenCastan.h \
    EdgeDetectMarrHill.h \
    EdgeDetectCommon.h \
    EdgeDetectCanny.h \
    OpCommons.h \
    imgwindow.h \
    Funcoes.h \
    filtros.h \
    qcustomplot.h \
    ocr.h \
    aba.h \
    thinning.h \
    ocrcaractere.h \
    classificadores.h \
    ocrwindow.h

SOURCES += mainwindow.cpp \
    main.cpp \
    LabelImage.cpp \
    EdgeDetectShenCastan.cpp \
    EdgeDetectMarrHill.cpp \
    EdgeDetectCommon.cpp \
    EdgeDetectCanny.cpp \
    OpCommons.cpp \
    imgwindow.cpp \
    Funcoes.cpp \
    filtros.cpp \
    qcustomplot.cpp \
    ocr.cpp \
    aba.cpp \
    thinning.cpp \
    ocrcaractere.cpp \
    classificadores.cpp \
    ocrwindow.cpp

FORMS += mainwindow.ui \
    imgwindow.ui \
    ocrwindow.ui

RESOURCES += hIP_Recursos.qrc
