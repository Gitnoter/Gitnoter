#ifndef LIBGIT2WRAPPER_GLOBAL_H
#define LIBGIT2WRAPPER_GLOBAL_H

#include <QtCore/qglobal.h>
/**
 * @todo lecserélni Qt-független implementációra
 */
#if defined(LIBGIT2WRAPPER_LIBRARY)
#  define LIBGIT2WRAPPERSHARED_EXPORT Q_DECL_EXPORT
#else
#  define LIBGIT2WRAPPERSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // LIBGIT2WRAPPER_GLOBAL_H
