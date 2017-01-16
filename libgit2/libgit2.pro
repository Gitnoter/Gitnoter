# qmake project file to build the static library libgit2

include(../qmake/common.pri)

QT -= gui
TARGET = libgit2
TEMPLATE = lib
CONFIG += staticlib

# note: paths relative to _PRO_FILE_PWD_ which is the profile path of
# the subprojects in src/*
INCLUDEPATH += \
  include \
  src \
  deps/zlib \
  deps/http-parser \

contains(QT_ARCH, .*64.*) {
    DEFINES += GIT_ARCH_64
} else {
    DEFINES += GIT_ARCH_32
}

DEFINES += STDC
DEFINES += _FILE_OFFSET_BITS=64

# Currently qmake (version 3.0) does not remove a library
# target if the destination directory is not the build directory,
# so we try to add this. Unfortunately it may be non-platform-portable
# as we need to guess the real filename.
# See target 'distclean' in build/app-lib/Makefile
QMAKE_DISTCLEAN += $${DESTDIR}/$${QMAKE_PREFIX_STATICLIB}$${TARGET}.$${QMAKE_EXTENSION_STATICLIB}

*-g++ {
    # standard GNU c++ behaviour is to complain about this, but the
    # sources are not prepared to fullfill all initializations
    # Some flags just taken from the original CMakeLists.txt settings
    QMAKE_CFLAGS += \
        -Wno-missing-field-initializers \
        -Wstrict-aliasing=2 \
        -Wstrict-prototypes \
        -Wdeclaration-after-statement \
        -Wno-unused-const-variable \
        -Wno-unused-function \
        -Wno-parentheses \
        -Wno-unused-parameter \
        -Wno-implicit-function-declaration \

    DEFINES += _GNU_SOURCE
}

win32-g++ {
  DEFINES += WIN32
  DEFINES += _WIN32_WINNT=0x0501


  INCLUDEPATH += \
    deps/regex \

  # Unfortunately, QtCreator parses these entries even on non-Window
  # systems - so the files are shown in the project tree, although
  # they will not be included into the generated library.

  HEADERS += \
    ./deps/regex/config.h \
    ./deps/regex/regex.h \
    ./deps/regex/regex_internal.h \
    ./src/win32/dir.h \
    ./src/win32/error.h \
    ./src/win32/findfile.h \
    ./src/win32/mingw-compat.h \
    ./src/win32/msvc-compat.h \
    ./src/win32/path_w32.h \
    ./src/win32/posix.h \
    ./src/win32/precompiled.h \
    ./src/win32/pthread.h \
    ./src/win32/reparse.h \
    ./src/win32/utf-conv.h \
    ./src/win32/version.h \
    ./src/win32/w32_util.h \

  SOURCES += \
    ./deps/regex/regcomp.c \
    ./deps/regex/regex.c \
    ./deps/regex/regexec.c \
    ./deps/regex/regex_internal.c \
    #
    ./src/win32/dir.c \
    ./src/win32/error.c \
    ./src/win32/findfile.c \
    ./src/win32/map.c \
    ./src/win32/path_w32.c \
    ./src/win32/posix_w32.c \
    ./src/win32/precompiled.c \
    ./src/win32/pthread.c \
    ./src/win32/utf-conv.c \
    ./src/win32/w32_util.c \

} else {
    # Non-Windows = Unix :)
  SOURCES += \
    ./src/unix/map.c \
    ./src/unix/realpath.c \

}

HEADERS += \
  ./deps/http-parser/http_parser.h \
  ./deps/zlib/crc32.h \
  ./deps/zlib/deflate.h \
  ./deps/zlib/inffast.h \
  ./deps/zlib/inffixed.h \
  ./deps/zlib/inflate.h \
  ./deps/zlib/inftrees.h \
  ./deps/zlib/trees.h \
  ./deps/zlib/zconf.h \
  ./deps/zlib/zlib.h \
  ./deps/zlib/zutil.h \
#
  ./include/git2/annotated_commit.h \
  ./include/git2/attr.h \
  ./include/git2/blame.h \
  ./include/git2/blob.h \
  ./include/git2/branch.h \
  ./include/git2/buffer.h \
  ./include/git2/checkout.h \
  ./include/git2/cherrypick.h \
  ./include/git2/clone.h \
  ./include/git2/commit.h \
  ./include/git2/common.h \
  ./include/git2/config.h \
  ./include/git2/cred_helpers.h \
  ./include/git2/describe.h \
  ./include/git2/diff.h \
  ./include/git2/errors.h \
  ./include/git2/filter.h \
  ./include/git2/global.h \
  ./include/git2/graph.h \
  ./include/git2.h \
  ./include/git2/ignore.h \
  ./include/git2/indexer.h \
  ./include/git2/index.h \
  ./include/git2/inttypes.h \
  ./include/git2/merge.h \
  ./include/git2/message.h \
  ./include/git2/net.h \
  ./include/git2/notes.h \
  ./include/git2/object.h \
  ./include/git2/odb_backend.h \
  ./include/git2/odb.h \
  ./include/git2/oidarray.h \
  ./include/git2/oid.h \
  ./include/git2/pack.h \
  ./include/git2/patch.h \
  ./include/git2/pathspec.h \
  ./include/git2/push.h \
  ./include/git2/rebase.h \
  ./include/git2/refdb.h \
  ./include/git2/reflog.h \
  ./include/git2/refs.h \
  ./include/git2/refspec.h \
  ./include/git2/remote.h \
  ./include/git2/repository.h \
  ./include/git2/reset.h \
  ./include/git2/revert.h \
  ./include/git2/revparse.h \
  ./include/git2/revwalk.h \
  ./include/git2/signature.h \
  ./include/git2/stash.h \
  ./include/git2/status.h \
  ./include/git2/stdint.h \
  ./include/git2/strarray.h \
  ./include/git2/submodule.h \
  ./include/git2/sys/commit.h \
  ./include/git2/sys/config.h \
  ./include/git2/sys/diff.h \
  ./include/git2/sys/filter.h \
  ./include/git2/sys/hashsig.h \
  ./include/git2/sys/index.h \
  ./include/git2/sys/mempack.h \
  ./include/git2/sys/odb_backend.h \
  ./include/git2/sys/openssl.h \
  ./include/git2/sys/refdb_backend.h \
  ./include/git2/sys/reflog.h \
  ./include/git2/sys/refs.h \
  ./include/git2/sys/repository.h \
  ./include/git2/sys/stream.h \
  ./include/git2/sys/transport.h \
  ./include/git2/tag.h \
  ./include/git2/trace.h \
  ./include/git2/transaction.h \
  ./include/git2/transport.h \
  ./include/git2/tree.h \
  ./include/git2/types.h \
  ./include/git2/version.h \
#
  ./src/annotated_commit.h \
  ./src/array.h \
  ./src/attrcache.h \
  ./src/attr_file.h \
  ./src/attr.h \
  ./src/bitvec.h \
  ./src/blame_git.h \
  ./src/blame.h \
  ./src/blob.h \
  ./src/branch.h \
  ./src/bswap.h \
  ./src/buffer.h \
  ./src/buf_text.h \
  ./src/cache.h \
  ./src/cc-compat.h \
  ./src/checkout.h \
  ./src/clone.h \
  ./src/commit.h \
  ./src/commit_list.h \
  ./src/common.h \
  ./src/config_file.h \
  ./src/config.h \
  ./src/delta-apply.h \
  ./src/delta.h \
  ./src/diff_driver.h \
  ./src/diff_file.h \
  ./src/diff.h \
  ./src/diff_patch.h \
  ./src/diff_xdiff.h \
  ./src/fetch.h \
  ./src/fetchhead.h \
  ./src/filebuf.h \
  ./src/fileops.h \
  ./src/filter.h \
  ./src/fnmatch.h \
  ./src/global.h \
  ./src/hash.h \
  ./src/hash/hash_common_crypto.h \
  ./src/hash/hash_generic.h \
  ./src/hash/hash_openssl.h \
  ./src/ignore.h \
  ./src/index.h \
  ./src/iterator.h \
  ./src/khash.h \
  ./src/map.h \
  ./src/merge.h \
  ./src/merge_file.h \
  ./src/message.h \
  ./src/mwindow.h \
  ./src/netops.h \
  ./src/notes.h \
  ./src/object.h \
  ./src/odb.h \
  ./src/offmap.h \
  ./src/oidarray.h \
  ./src/oid.h \
  ./src/oidmap.h \
  ./src/openssl_stream.h \
  ./src/pack.h \
  ./src/pack-objects.h \
  ./src/path.h \
  ./src/pathspec.h \
  ./src/pool.h \
  ./src/posix.h \
  ./src/pqueue.h \
  ./src/push.h \
  ./src/refdb_fs.h \
  ./src/refdb.h \
  ./src/reflog.h \
  ./src/refs.h \
  ./src/refspec.h \
  ./src/remote.h \
  ./src/repository.h \
  ./src/repo_template.h \
  ./src/revwalk.h \
  ./src/sha1_lookup.h \
  ./src/signature.h \
  ./src/socket_stream.h \
  ./src/sortedcache.h \
  ./src/status.h \
  ./src/stream.h \
  ./src/strmap.h \
  ./src/strnlen.h \
  ./src/submodule.h \
  ./src/sysdir.h \
  ./src/tag.h \
  ./src/thread-utils.h \
  ./src/trace.h \
  ./src/transports/auth.h \
  ./src/transports/auth_negotiate.h \
  ./src/transports/cred.h \
  ./src/transports/smart.h \
  ./src/tree-cache.h \
  ./src/tree.h \
  ./src/unix/posix.h \
  ./src/userdiff.h \
  ./src/util.h \
  ./src/vector.h \
  ./src/xdiff/xdiff.h \
  ./src/xdiff/xdiffi.h \
  ./src/xdiff/xemit.h \
  ./src/xdiff/xinclude.h \
  ./src/xdiff/xmacros.h \
  ./src/xdiff/xprepare.h \
  ./src/xdiff/xtypes.h \
  ./src/xdiff/xutils.h \
  ./src/zstream.h \


SOURCES += \
  ./src/annotated_commit.c \
  ./src/attr.c \
  ./src/attrcache.c \
  ./src/attr_file.c \
  ./src/blame.c \
  ./src/blame_git.c \
  ./src/blob.c \
  ./src/branch.c \
  ./src/buffer.c \
  ./src/buf_text.c \
  ./src/cache.c \
  ./src/checkout.c \
  ./src/cherrypick.c \
  ./src/clone.c \
  ./src/commit.c \
  ./src/commit_list.c \
  ./src/config.c \
  ./src/config_cache.c \
  ./src/config_file.c \
  ./src/crlf.c \
  ./src/date.c \
  ./src/delta-apply.c \
  ./src/delta.c \
  ./src/describe.c \
  ./src/diff.c \
  ./src/diff_driver.c \
  ./src/diff_file.c \
  ./src/diff_patch.c \
  ./src/diff_print.c \
  ./src/diff_stats.c \
  ./src/diff_tform.c \
  ./src/diff_xdiff.c \
  ./src/errors.c \
  ./src/fetch.c \
  ./src/fetchhead.c \
  ./src/filebuf.c \
  ./src/fileops.c \
  ./src/filter.c \
  ./src/fnmatch.c \
  ./src/global.c \
  ./src/graph.c \
  ./src/hash.c \
  ./src/hashsig.c \
  ./src/ident.c \
  ./src/ignore.c \
  ./src/index.c \
  ./src/indexer.c \
  ./src/iterator.c \
  ./src/merge.c \
  ./src/merge_file.c \
  ./src/message.c \
  ./src/mwindow.c \
  ./src/netops.c \
  ./src/notes.c \
  ./src/object_api.c \
  ./src/object.c \
  ./src/odb.c \
  ./src/odb_loose.c \
  ./src/odb_mempack.c \
  ./src/odb_pack.c \
  ./src/oidarray.c \
  ./src/oid.c \
  ./src/openssl_stream.c \
  ./src/pack.c \
  ./src/pack-objects.c \
  ./src/path.c \
  ./src/pathspec.c \
  ./src/pool.c \
  ./src/posix.c \
  ./src/pqueue.c \
  ./src/push.c \
  ./src/rebase.c \
  ./src/refdb.c \
  ./src/refdb_fs.c \
  ./src/reflog.c \
  ./src/refs.c \
  ./src/refspec.c \
  ./src/remote.c \
  ./src/repository.c \
  ./src/reset.c \
  ./src/revert.c \
  ./src/revparse.c \
  ./src/revwalk.c \
  ./src/settings.c \
  ./src/sha1_lookup.c \
  ./src/signature.c \
  ./src/socket_stream.c \
  ./src/sortedcache.c \
  ./src/stash.c \
  ./src/status.c \
  ./src/strmap.c \
  ./src/submodule.c \
  ./src/sysdir.c \
  ./src/tag.c \
  ./src/thread-utils.c \
  ./src/trace.c \
  ./src/transaction.c \
  ./src/transport.c \
  ./src/tree.c \
  ./src/tree-cache.c \
  ./src/tsort.c \
  ./src/util.c \
  ./src/vector.c \
  ./src/zstream.c \
#
  ./deps/http-parser/http_parser.c \
#
  ./src/hash/hash_generic.c \
#
  ./src/transports/auth.c \
  ./src/transports/auth_negotiate.c \
  ./src/transports/cred.c \
  ./src/transports/cred_helpers.c \
  ./src/transports/git.c \
  ./src/transports/http.c \
  ./src/transports/local.c \
  ./src/transports/smart.c \
  ./src/transports/smart_pkt.c \
  ./src/transports/smart_protocol.c \
  ./src/transports/ssh.c \
  ./src/transports/winhttp.c \
#
  ./src/xdiff/xdiffi.c \
  ./src/xdiff/xemit.c \
  ./src/xdiff/xhistogram.c \
  ./src/xdiff/xmerge.c \
  ./src/xdiff/xpatience.c \
  ./src/xdiff/xprepare.c \
  ./src/xdiff/xutils.c \
#
  ./deps/zlib/adler32.c \
  ./deps/zlib/crc32.c \
  ./deps/zlib/deflate.c \
  ./deps/zlib/infback.c \
  ./deps/zlib/inffast.c \
  ./deps/zlib/inflate.c \
  ./deps/zlib/inftrees.c \
  ./deps/zlib/trees.c \
  ./deps/zlib/zutil.c \


OTHER_FILES += \
  # these are not intended for MINGW, see CMakeLists.txt
  ./src/hash/hash_win32.h \
  ./src/hash/hash_win32.c \
  # examples and tests
  ./examples/common.h \
  ./examples/network/common.h \
  \
  ./examples/add.c \
  ./examples/blame.c \
  ./examples/cat-file.c \
  ./examples/common.c \
  ./examples/describe.c \
  ./examples/diff.c \
  ./examples/for-each-ref.c \
  ./examples/general.c \
  ./examples/init.c \
  ./examples/log.c \
  ./examples/network/clone.c \
  ./examples/network/common.c \
  ./examples/network/fetch.c \
  ./examples/network/git2.c \
  ./examples/network/index-pack.c \
  ./examples/network/ls-remote.c \
  ./examples/remote.c \
  ./examples/rev-list.c \
  ./examples/rev-parse.c \
  ./examples/showindex.c \
  ./examples/status.c \
  ./examples/tag.c \
  \
