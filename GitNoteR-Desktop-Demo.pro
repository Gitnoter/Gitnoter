# 表明这是一个子项目
TEMPLATE = subdirs

# 添加子项目
SUBDIRS += \
    GitNoteR \
    libgit2 \
    libqgit2

# 编译 GitNoteR 前先编译 libgit2、libqgit2
GitNoteR.depends = libgit2 libqgit2
