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

`src/3rdparty/libgit2/lib`下的二进制库需要更改搜索路径, 否则运行时将会找不到

## 参考项目

- [cloose/CuteMarkEd](https://github.com/cloose/CuteMarkEd)
- [pbek/QOwnNotes](https://github.com/pbek/QOwnNotes)

## 使用

### Gitnoter 主界面说明

![主界面说明](https://raw.githubusercontent.com/Gitnoter/Gitnoter-Qt-Desktop/master/resources/1.png)

- ① 中横线前面显示笔记名称, 后面显示归档, [文档标题 - 归档]
- ② 系统管理的归档文件夹
    + 所有笔记
    + 最近 7 天使用
    + 未分类的笔记
    + 回收站
- ③ 归档文件夹列表, 点击可进入笔记列表
- ④ 标签文件夹列表, 点击可进入笔记列表
- ⑤ 点击加号添加归档或标签文件夹, 点击减号删除归档或标签文件夹, 添加或删除前需要选中对应的区域
- ⑥ 搜索当前列表的笔记
- ⑦ 当前说选中的归档文件夹的名称和数量
- ⑧ 排序, 可按照标题, 创建时间, 更新时间, 升序降序排序
- ⑨ 选中分区后的笔记列表
- ⑩ 点击加号添加笔记, 点击减号删除笔记; 如果当前选中的是在回收站中, 那么点击加号还原笔记, 点击减号彻底删除笔记, 长按加号还原所有笔记, 长按减号清空回收站
- ⑪ 点击打开修改归档文件夹面板修改归档文件夹
- ⑫ 输入标签名回车确定添加标签, 点击已输入的标签可删除标签
- ⑬ 编辑|预览模式按钮[CMD + R], 预览模式按钮[CMD + P]
- ⑭ 工具栏
- ⑮ 笔记编辑界面
- ⑯ Markdown 预览界面
- ⑰ Markdown 导航界面, 点击可跳到指定的编辑位置

### 添加 Git 仓库

打开设置面板, macOS `CMD + ,`, Linux 和 Windows `笔记 -> 设置...`

![](https://raw.githubusercontent.com/Gitnoter/Gitnoter-Qt-Desktop/master/resources/2.jpg)

- 仓库地址: 注册 `Git` 平台账号后创建仓库得到, 目前不支持`SSH`, 建议使用私有仓库
    + 国内建议使用: [Coding](https://coding.net/), [oschina](https://gitee.com)
- 登录用户: 注册 `Git` 平台时的账户名, 一般为邮箱
- 登录密码: 注册 `Git` 平台时的登录密码

### 定时锁屏

打开设置面板, macOS `CMD + ,`, Linux 和 Windows `笔记 -> 设置...`

![](https://raw.githubusercontent.com/Gitnoter/Gitnoter-Qt-Desktop/master/resources/3.jpg)

- 当无任何操作时自动锁定: 设置自动锁屏的延时时间
- 解锁密码: 设置解锁密码, 不设置为空

![](https://raw.githubusercontent.com/Gitnoter/Gitnoter-Qt-Desktop/master/resources/4.jpg)

- 输入正确密码后如果为成解锁, 请再次输入回车

## 许可

`GPLv3.0` 更多信息请查看`LICENSE`文件

