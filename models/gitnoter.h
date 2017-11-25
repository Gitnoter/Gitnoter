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

    enum MenuBarActionFlag {
        MenuNotes = 0x004001,
        ActionNewNote = 0x004002,
        ActionNewCategories = 0x004003,
        ActionNewTags = 0x004004,
        ActionSaveNote = 0x004005,
        ActionSynchNote = 0x004006,
        ActionReloadNotes = 0x004007,
        ActionFindWithFolder = 0x004008,
        ActionImportEvernote = 0x004009,
        ActionExportHtmlForPDF = 0x00400a,
        ActionExportMarkdownForPDF = 0x00400b,
        ActionExportHtml = 0x00400c,
        ActionExportMarkdown = 0x00400d,
        ActionPrintPageSetting = 0x00400e,
        ActionPrint = 0x00400f,
        ActionPreferences = 0x004010,


        MenuEdit = 0x004011,
        ActionRepeal = 0x004012,
        ActionReform = 0x004013,
        ActionCut = 0x004014,
        ActionCopy = 0x004015,
        ActionPaste = 0x004016,
        ActionPasteHtml = 0x004017,
        ActionSelectAll = 0x004018,
        ActionDeleteSelected = 0x004019,
        ActionDeleteNote = 0x00401a,
        ActionCopyLine = 0x00401b,
        ActionDeleteLine = 0x00401c,
        ActionWebSearch = 0x00401d,

        MenuSearch = 0x00401e,
        ActionGroupSearch = 0x00401f,
        ActionFindText = 0x004020,
        ActionFindNext = 0x004021,
        ActionFindLast = 0x004022,
        ActionReplaceText = 0x004023,
        ActionReplaceAndNext = 0x004024,
        ActionReplaceAll = 0x004025,

        MenuSwitch = 0x004026,
        ActionToUppercase = 0x004027,
        ActionToLowercase = 0x004028,
        ActionToUppercaseAtFirst = 0x004029,


        MenuSign = 0x00402a,
        ActionTitle1 = 0x00402b,
        ActionTitle2 = 0x00402c,
        ActionTitle3 = 0x00402d,
        ActionTitle4 = 0x00402e,
        ActionTitle5 = 0x00402f,
        ActionStrong = 0x004030,
        ActionIncline = 0x004031,
        ActionStrickout = 0x004032,
        ActionUnderline = 0x004033,
        ActionInnerCodeBlock = 0x004034,
        ActionCodeBlock = 0x004035,
        ActionAnnotation = 0x004036,
        ActionMoreAnnotation = 0x004037,
        ActionLink = 0x004038,
        ActionLinkImage = 0x004039,
        ActionInsertImage = 0x00403a,
        ActionAccessory = 0x00403b,
        ActionTable = 0x00403c,
        ActionOrderedList = 0x00403d,
        ActionUnorderedList = 0x00403e,
        ActionQuoteBlock = 0x00403f,
        ActionCuttingLine = 0x004040,
        ActionNowTime = 0x004041,

        MenuView = 0x004042,
        ActionSidebar = 0x004043,
        ActionListbar = 0x004044,
        ActionToolbar = 0x004045,
        ActionNavigationBar = 0x004046,
        ActionEditMode = 0x004047,
        ActionPreviewMode = 0x004048,
        ActionEditPreviewMode = 0x004049,
        ActionNightMode = 0x00404a,
        ActionAddFontSize = 0x00404b,
        ActionSubtractFontSize = 0x00404c,
        ActionResetFontSize = 0x00404d,
        ActionEnterFullScreen = 0x00404e,

        MenuWindow = 0x00404f,
        ActionMixWindow = 0x004050,
        ActionResizeWindow = 0x004051,
        ActionNewWindow = 0x004052,
        ActionNewEditWindow = 0x004053,
        ActionShowLastWindow = 0x004054,
        ActionShowNextWindow = 0x004055,
        ActionCloseWindow = 0x004056,
        ActionCloseAllWindow = 0x004057,
        ActionLock = 0x004058,
        ActionPrepose = 0x004059,

        MenuHelp = 0x00405a,
        ActionGuide = 0x00405b,
        ActionHistory = 0x00405c,
        ActionMarkdownLanguage = 0x00405d,
        ActionFeedback = 0x00405e,
        ActionIssue = 0x00405f,
        ActionAbout = 0x004060
    };
    Q_DECLARE_FLAGS(MenuBarActionFlags, MenuBarActionFlag)
    Q_FLAGS(MenuBarActionFlags)

};

Q_DECLARE_OPERATORS_FOR_FLAGS(Gitnoter::UpdateViewFlags)
Q_DECLARE_OPERATORS_FOR_FLAGS(Gitnoter::MenuBarActionFlags)

#endif // GITNOTER_H
