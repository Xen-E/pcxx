QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# object_parallel_to_source is important because we have files with same names
# it will put generated objects in different directories.
CONFIG += c++11 object_parallel_to_source

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


QMAKE_CXXFLAGS_RELEASE += -O3 # Otimize for code size and execution time
QMAKE_CXXFLAGS_DEBUG += -O0 # Optimize for compilation time


INCLUDEPATH += include # Includes "include" directory/folder


# This section will try to include the taglib library
# based on platform and architecture
# change if necessary or you have installed the library in a different location

win32 { # If the platform is windows
    x86_64 { # 64bit
	    message("Building for x86_64 (64bit) windows")
		win32:INCLUDEPATH += \
		    C:/Libraries/taglib-1.11.1/lib/gcc_x64_lib/include
		win32:LIBS += \
		    -LC:/Libraries/taglib-1.11.1/lib/gcc_x64_lib/lib -ltag -lz
	}
	i686 { # 32bit
	    message("Building for i686 (32bit) windows")
		win32:INCLUDEPATH += \
		    C:/Libraries/taglib-1.11.1/lib/gcc_i686_lib/include
		win32:LIBS += \
		    -LC:/Libraries/taglib-1.11.1/lib/gcc_i686_lib/lib -ltag -lz
	}
}

# If the platform is *nix (Linux, Darwin..etc)
unix:INCLUDEPATH += \
    /usr/include/taglib

unix:LIBS += \
    -ltag

# Source files
SOURCES += \
    src/DirFilter.cpp \
    src/Export.cpp \
    src/FileReader.cpp \
    src/FileSize.cpp \
    src/ID3Fixer.cpp \
    src/IDHandler.cpp \
    src/PCxx.cpp \
    src/Parse.cpp \
    src/PlaylistManager.cpp \
    src/ShowInFolder.cpp \
    src/TimeFormatter.cpp \
    src/formatters/ASX.cpp \
    src/formatters/B4S.cpp \
    src/formatters/HTML.cpp \
    src/formatters/JSPF.cpp \
    src/formatters/M3U.cpp \
    src/formatters/MPCPL.cpp \
    src/formatters/PLS.cpp \
    src/formatters/TXT.cpp \
    src/formatters/VDJ.cpp \
    src/formatters/WPL.cpp \
    src/formatters/XSPF.cpp \
    src/main.cpp \
    src/parsers/AIMPPL.cpp \
    src/parsers/ASX.cpp \
    src/parsers/B4S.cpp \
    src/parsers/FPL.cpp \
    src/parsers/ITUNES.cpp \
    src/parsers/JSPF.cpp \
    src/parsers/M3U.cpp \
    src/parsers/MPCPL.cpp \
    src/parsers/PLS.cpp \
    src/parsers/TXT.cpp \
    src/parsers/WPL.cpp \
    src/parsers/XSPF.cpp

# Include files
HEADERS += \
    include/aboutAnimator.h \
    include/exporter.h \
    include/fileReader.h \
    include/id3Fixer.h \
    include/misc.h \
    include/parser.h \
	include/pcxx.h \
    include/types.h \
    include/versions.h


# Because we have a static version of taglib this is important
# you can remove it if you're using a dynamic version
DEFINES += \
    TAGLIB_STATIC

FORMS += \
    pcxx.ui

RESOURCES += \
    resources.qrc

# This is for windows only,
# It will provide some information for the produced binary.
# It can be seen in "Detials" section in "Properties" window and some other places.
win32:QMAKE_TARGET_COMPANY = "Xen"
win32:QMAKE_TARGET_PRODUCT = "Playlist Converter++"
win32:QMAKE_TARGET_DESCRIPTION = "Convert, Manage & Make professional playlists."
win32:QMAKE_TARGET_COPYRIGHT = "Xen 2019 <xen-dev@pm.me>"
win32:RC_ICONS = icons/pcxx.ico
win32:VERSION = 1.1.0.0
else:VERSION = 1.1.0
