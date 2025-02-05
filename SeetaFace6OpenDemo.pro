QT       += core gui multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwidget.cpp \
    matchwidget.cpp \
    qtcameracapture.cpp

HEADERS += \
    mainwidget.h \
    matchwidget.h \
    qtcameracapture.h

FORMS += \
    mainwidget.ui \
    matchwidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RC_ICONS += icon.ico

include(SeetaFace6Open_x64/SeetaFace6Open_x64.pri)
