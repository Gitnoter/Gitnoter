#ifndef MENUBAR_H
#define MENUBAR_H

#include "mainwindow.h"
#include "loginwidget.h"
#include "markdowneditorwidget.h"

#include <QMainWindow>
#include <QPrinter>
#include <QActionGroup>

namespace Ui {
class MenuBar;
}

class MarkdownEditorWidget;

class MenuBar : public QMainWindow
{
    Q_OBJECT

public:
    explicit MenuBar(QWidget *parent = nullptr, MainWindow *mainWindow = nullptr);
    ~MenuBar();

    void setGroupEnable();
    void addActionToWindowMenu(QWidget *widget = 0);
    void removeActionToWindowMenu(QWidget *widget = 0);

    void initLicenseAction(bool license = false);
    void addHasLicenseAction();
    void addNoLicenseAction();

//    void initFormMarkdownEditorWidget();
//    void initFormGroupTreeWidget();
//    void initFormNoteListWidget();

private:
    void setupUi(QWidget *parent);

signals:
    void printEditAccepted(QPrinter *printer);
    void printPreviewAccepted(QPrinter *printer);

public slots:
    void initGlobalHotKeys();

private slots:
    void onPrintEditAccepted();
    void onPrintPreviewAccepted();
    void onWindowMenuActionTriggered();

    void on_action_printPageSetting_triggered();
    void on_action_printEdit_triggered();
    void on_action_printPreview_triggered();

public:
    QAction *getActionNewNote() const;
    QAction *getActionNewCategories() const;
    QAction *getActionNewTags() const;
    QAction *getActionSaveNote() const;
    QAction *getActionSynchNote() const;
    QAction *getActionUndo() const;
    QAction *getActionRedo() const;
    QAction *getActionCut() const;
    QAction *getActionCopy() const;
    QAction *getActionPaste() const;
    QAction *getActionSelectAll() const;
    QAction *getActionGroupSearch() const;
    QAction *getActionFindText() const;
    QAction *getActionToUppercase() const;
    QAction *getActionToLowercase() const;
    QAction *getActionToUppercaseAtFirst() const;
    QAction *getActionDeleteNote() const;
    QAction *getActionGuide() const;
    QAction *getActionMarkdownLanguage() const;
    QAction *getActionFeedback() const;
    QAction *getActionHistory() const;
    QAction *getActionIssue() const;
    QAction *getActionPreposeWindow() const;
    QAction *getActionMixWindow() const;
    QAction *getActionResizeWindow() const;
    QAction *getActionEnterFullScreen() const;
    QAction *getActionAbout() const;
    QAction *getActionPreferences() const;
    QAction *getActionFindNext() const;
    QAction *getActionFindLast() const;
    QAction *getActionReplaceText() const;
    QAction *getActionReplaceAndNext() const;
    QAction *getActionReplaceAll() const;
    QAction *getActionPasteHtml() const;
    QAction *getActionWebSearch() const;
    QAction *getActionCopyLine() const;
    QAction *getActionDeleteLine() const;
    QAction *getActionDeleteText() const;
    QAction *getActionClearText() const;
    QAction *getActionTitle1() const;
    QAction *getActionTitle2() const;
    QAction *getActionTitle3() const;
    QAction *getActionTitle4() const;
    QAction *getActionTitle5() const;
    QAction *getActionStrong() const;
    QAction *getActionItalic() const;
    QAction *getActionStrickout() const;
    QAction *getActionUnderline() const;
    QAction *getActionTable() const;
    QAction *getActionOrderedList() const;
    QAction *getActionUnorderedList() const;
    QAction *getActionTaskList() const;
    QAction *getActionLink() const;
    QAction *getActionLinkImage() const;
    QAction *getActionInsertImage() const;
    QAction *getActionAccessory() const;
    QAction *getActionQuoteBlock() const;
    QAction *getActionCuttingLine() const;
    QAction *getActionNowTime() const;
    QAction *getActionInnerCodeBlock() const;
    QAction *getActionCodeBlock() const;
    QAction *getActionAnnotation() const;
    QAction *getActionMoreAnnotation() const;
    QAction *getActionNewWindow() const;
    QAction *getActionCloseWindow() const;
    QAction *getActionCloseAllWindow() const;
    QAction *getActionReloadNotes() const;
    QAction *getActionFindWithFolder() const;
    QAction *getActionImportEvernote() const;
    QAction *getActionExportHtmlForPDF() const;
    QAction *getActionExportMarkdownForPDF() const;
    QAction *getActionExportHtml() const;
    QAction *getActionExportMarkdown() const;
    QAction *getActionPrintPageSetting() const;
    QAction *getActionPrintEdit() const;
    QAction *getActionPrintPreview() const;
    QAction *getActionSidebar() const;
    QAction *getActionListbar() const;
    QAction *getActionToolbar() const;
    QAction *getActionNavigationBar() const;
    QAction *getActionPreviewMode() const;
    QAction *getActionEditMode() const;
    QAction *getActionEditPreviewMode() const;
    QAction *getActionFullScreenEditMode() const;
    QAction *getActionNightMode() const;
    QAction *getActionPlusFontSize() const;
    QAction *getActionSubtractFontSize() const;
    QAction *getActionResetFontSize() const;
    QAction *getActionLockWindow() const;
    QAction *getActionNewEditWindow() const;
    QAction *getActionShowLastWindow() const;
    QAction *getActionShowNextWindow() const;

    QActionGroup *windowMenuActionGroup() { return mWindowMenuActionGroup; }
    QWidgetList windowMenuWidgetList();

    void setMainWindow(MainWindow *mainWindow) { mMainWindow = mainWindow; }

private:
    Ui::MenuBar *ui;
    QPrinter *mPrinter;
    QActionGroup *mWindowMenuActionGroup;
    MainWindow *mMainWindow;

};

#endif // MENUBAR_H
