# Gitnoter

基于`Git`仓库存储的`Markdown`笔记应用

## 开发始终

始: 2016 年印象笔记更改付费策略, 只能同时绑定两个设备
终: 大概在几天前, 看到了`tamlok/vnote`这项目, 下载体验后, 我决定放弃这个项目(弃坑咯)

## 安装

- [macOS](https://gitee.com/Gitnoter/Gitnoter/attach_files/download?i=115052&u=http%3A%2F%2Ffiles.git.oschina.net%2Fgroup1%2FM00%2F02%2FC7%2FPaAvDFpgPD6ADQTMAMK6Bcw3HE4841.dmg%3Ftoken%3Ded2b4365477f20a91ab3393642e92162%26ts%3D1553070875%26attname%3DGitnoter-osx-v1.0.0.beta7.dmg) 双击打开后拖动应用到`Appactions`里
- [linux](https://gitee.com/Gitnoter/Gitnoter/attach_files/download?i=115054&u=http%3A%2F%2Ffiles.git.oschina.net%2Fgroup1%2FM00%2F02%2FC7%2FPaAvDFpgPsiAVxYzAb2x4V-_ALw1308.gz%3Ftoken%3D79b6a4eae1e7a540b3a8b390047101de%26ts%3D1553070875%26attname%3DGitnoter-linux-v1.0.0.beta7.tar.gz) 解压后运行`AppRun`
- [windows](https://gitee.com/Gitnoter/Gitnoter/attach_files/download?i=115053&u=http%3A%2F%2Ffiles.git.oschina.net%2Fgroup1%2FM00%2F02%2FC7%2FPaAvDFpgPEeAAmJoASje1BZ1vfE302.zip%3Ftoken%3Dfb6ad660e015867893d7a772ab4a689a%26ts%3D1553070875%26attname%3DGitnoter-windows-v1.0.0.beta7.zip) 解压后运行`Gitnoter.exe`

## 开发

开发主要基于`QMake`进行, 也可使用`CMake`(CMakeList.txt已很久没维护了)

### 依赖

- [Qt](https://www.qt.io/)
- [QMarkdownTextEdit](https://github.com/pbek/qmarkdowntextedit)
- [QKeySequenceWidget](https://github.com/DOOMer/qkeysequencewidget)
- [QTinyAes](https://github.com/Skycoder42/QTinyAes)
- [libgit2](https://github.com/libgit2/libgit2)
- [hoedown](https://github.com/hoedown/hoedown)

### 分支管理

- `master`分支为公共代码分支
- `build-windows`分支为针对`windows`平台进行优化的分支
- `build-macOS`分支为针对`macOS`平台进行优化的分支
- `build-linux`分支为针对`linux`平台进行优化的分支

### 注意

`src/3rdparty/libGit2/lib`下的二进制库需要重新跟搜索路径, 否则运行时将会找不到

## 参考项目

- [cloose/CuteMarkEd](https://github.com/cloose/CuteMarkEd)
- [pbek/QOwnNotes](https://github.com/pbek/QOwnNotes)

## 许可

`GPLv3.0` 更多信息请查看`LICENSE`文件

