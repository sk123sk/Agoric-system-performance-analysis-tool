QT -= gui
QT += core network


CONFIG += c++11 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        CheckNodeWorker.cpp \
        main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:LIBS += -L$$PWD/ -lcrypt32 -ldnsapi -lws2_32

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../chilkat/chilkat-9.5.0-x86_64-8.1.0-posix-seh-rt_v6-rev0/ -llibchilkat-9.5.0

INCLUDEPATH += $$PWD/../chilkat/chilkat-9.5.0-x86_64-8.1.0-posix-seh-rt_v6-rev0/include
DEPENDPATH += $$PWD/../chilkat/chilkat-9.5.0-x86_64-8.1.0-posix-seh-rt_v6-rev0/include

LIBS += -L$$PWD/lib/openssl/ -lssl-1_1-x64
LIBS += -L$$PWD/lib/openssl/ -lcrypto-1_1-x64

EXTRA_BINFILES += \
       $$PWD/lib/openssl/libcrypto-1_1-x64.dll \
       $$PWD/lib/openssl/libssl-1_1-x64.dll

EXTRA_BINFILES_WIN = $${EXTRA_BINFILES}
EXTRA_BINFILES_WIN ~= s,/,\\,g

CONFIG(release, debug|release): DESTDIR_WIN = release
CONFIG(debug, debug|release): DESTDIR_WIN = debug
for(FILE,EXTRA_BINFILES_WIN) {
    QMAKE_POST_LINK +=$$quote(cmd /c copy /y $${FILE} $${DESTDIR_WIN}$$escape_expand(\n\t))
}

INCLUDEPATH += $$PWD/include

HEADERS += \
    CheckNodeWorker.h
