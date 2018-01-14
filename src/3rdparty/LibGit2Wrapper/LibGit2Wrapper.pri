INCLUDEPATH += $$PWD/

HEADERS += \
    $$PWD/Authenticable.h     \
    $$PWD/LibGitInitializer.h     \
    $$PWD/Commit.h     \
    $$PWD/Diff.h     \
    $$PWD/Reference.h     \
    $$PWD/Repository.h     \
    $$PWD/Remote.h     \
    $$PWD/GitEnvironment.h     \
    $$PWD/RepositoryDependant.h     \
    $$PWD/LibGitException.h     \
    $$PWD/Exception.h     \
    $$PWD/Tag.h     \
    $$PWD/libgit2wrapper_global.h     \
    $$PWD/Branch.h


SOURCES += \
    $$PWD/Commit.cpp     \
    $$PWD/Diff.cpp     \
    $$PWD/GitEnvironment.cpp     \
    $$PWD/LibGitInitializer.cpp     \
    $$PWD/Reference.cpp     \
    $$PWD/Remote.cpp     \
    $$PWD/Repository.cpp     \
    $$PWD/RepositoryDependant.cpp     \
    $$PWD/SmartLibGitHeapPointerTemplate.hpp     \
    $$PWD/Tag.cpp     \
    $$PWD/Authenticable.cpp     \
    $$PWD/Branch.cpp

OTHER_FILES += \
    $$PWD/README.md     \
    $$PWD/LICENSE
