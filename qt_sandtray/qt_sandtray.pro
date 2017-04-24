HEADERS += \
    category.h \
    dragimage.h \
    librarymanager.h \
    checkcategorisation.h \
    gameengine.h \
    gamedata.h \
    urbisend.h \
    messages.h \
    urbireceive.h \
    bezierclass.h \
    librarybutton.h \
    refreshscreen.h

SOURCES += \
	main.cpp \
    category.cpp \
    dragimage.cpp \
    librarymanager.cpp \
    checkcategorisation.cpp \
    gameengine.cpp \
    gamedata.cpp \
    urbisend.cpp \
    urbireceive.cpp \
    bezierclass.cpp \
    librarybutton.cpp \
    refreshscreen.cpp

QT += network
QT += phonon

# FOR ADDING AN ICON IN WINDOWS
win32:RC_FILE += sandtrayicon.rc

# USED FOR LINUX TO FIND PHONON - NECESSARY!?
#INCLUDEPATH += /usr/lib/i386-linux-gnu
#LIBS += -lphonon

# USED TO BUILD BASED ON STATIC LIBS - CAN PRODUCE A SINGLE EXE TO RUN - doesn't work with phonon!
#CONFIG += static
