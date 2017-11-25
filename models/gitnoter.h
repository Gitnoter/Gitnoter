#ifndef GITNOTER_H
#define GITNOTER_H

#include <QObject>

class Gitnoter : public QObject
{
    Q_OBJECT
public:
    enum GroupType {
        All = 0x000001,
        Recent = 0x000002,
        Unclassified = 0x000003,
        Trash = 0x000004,
        Category = 0x000006,
        Tag = 0x000008
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
    
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Gitnoter::UpdateViewFlags)

#endif // GITNOTER_H
