#ifndef GITNOTER_H
#define GITNOTER_H

#include <QObject>

class Gitnoter : public QObject
{
    Q_OBJECT
public:
    enum GroupType {
        All = 0x000000,
        Recent = 0x000001,
        Unclassified = 0x000002,
        Trash = 0x000003,
        Category = 0x000004,
        Tag = 0x000005
    };

    enum SortBasis {
        Title = 0x001001,
        CreateDate = 0x001002,
        UpdateDate = 0x001003
    };

    enum UpdateViewFlag {
        MarkdownEditorWidget = 0x002001,
        NoteListTitle = 0x002002,
        NoteListWidget = 0x002003 | NoteListTitle | MarkdownEditorWidget,
        GroupTreeWidget = 0x002004 | NoteListWidget,
    };
    Q_DECLARE_FLAGS(UpdateViewFlags, UpdateViewFlag)
    Q_FLAGS(UpdateViewFlags)

    enum RepoStatus {
        NoneRepo = 0x003001,
        RemoteRepo = 0x003002,
        LocalRepo = 0x003003
    };

    enum GlobalHotKeys {
        CutRect,
        CutFull,
        CutWindow
    };

    enum MediaType {
        Image = 0x004001,
        Accessory = 0x004002
    };

    enum SingleTimerType {
        SingleTimeNone = 0x005001,
        ResetTimeout = 0x005002,
        NoResetTimeout = 0x005003
    };
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Gitnoter::UpdateViewFlags)

#endif // GITNOTER_H
