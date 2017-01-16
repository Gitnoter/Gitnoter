# qmake project includes for include paths etc.
#

include(../qmake/common.pri)

# The following part was inspired
# from http://stackoverflow.com/questions/2288292/qmake-project-dependencies-linked-libraries

# This function sets up the dependencies for libraries that are built with
# this project.  Specify the libraries you need to depend on in the variable
# DEPENDENCY_LIBRARIES and this will add.
for(dep, DEPENDENCY_LIBRARIES) {
    INCLUDEPATH += ../$${dep}
    LIBS += -L$${DESTDIR} -l$${dep}
    PRE_TARGETDEPS += $${DESTDIR}/$${QMAKE_PREFIX_STATICLIB}$${dep}.$${QMAKE_EXTENSION_STATICLIB}
}
