# qmake project include file with common settings

# The libs and executables shall not be "hidden" in the
# subdirectories of the build directory, therefore:
DESTDIR = ..            # target directory one level above

CONFIG += c++11         # enable features of C++11 standard
INCLUDEPATH += ..	# to allow includes of other libs

*-g++ {
    # -Wall / -Wextra are enabled by default
    #QMAKE_CXXFLAGS += -Werror
    QMAKE_CXXFLAGS += -Wdeprecated
    QMAKE_CXXFLAGS += -Wextra           # not enabled on linux by default
}
