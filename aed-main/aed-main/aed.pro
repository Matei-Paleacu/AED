QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
INCLUDEPATH += include/

SOURCES += \
    src/aed_device.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/state_manager.cpp

HEADERS += \
    include/shork/cs/aed_device.h \
    include/shork/cs/bit/self_test.h \
    include/shork/cs/intervention/compression.h \
    include/shork/cs/mainwindow.h \
    include/shork/cs/model/device_data.h \
    include/shork/cs/sensor/ecg.h \
    include/shork/cs/state_manager.h


FORMS += \
    form/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    resources/Heart.png

RESOURCES += \
    resource.qrc
