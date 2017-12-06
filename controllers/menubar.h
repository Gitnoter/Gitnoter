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
    explicit MenuBar(QWidget *parent = 0);
    ~MenuBar();

    void setGroupEnable();
    void setWindowMenu(QWidget *widget = 0);

//    void initFormMarkdownEditorWidget();
//    void initFormGroupTreeWidget();
//    void initFormNoteListWidget();

private:
    void setupUi(QWidget *parent);

signals:
    void printAccepted(QPrinter *printer);

private slots:
    void onPrintAccepted();
    void onWindowActionTriggered();

    void on_action_printPageSetting_triggered();
    void on_action_print_triggered();

    void on_action_importEvernote_triggered();
    void on_action_exportHtmlForPDF_triggered();
    void on_action_exportMarkdownForPDF_triggered();
    void on_action_exportHtml_triggered();
    void on_action_exportMarkdown_triggered();

    void on_action_title1_triggered();
    void on_action_title2_triggered();
    void on_action_title3_triggered();
    void on_action_title4_triggered();
    void on_action_title5_triggered();
    void on_action_strong_triggered();
    void on_action_incline_triggered();
    void on_action_strickout_triggered();
    void on_action_underline_triggered();
    void on_action_innerCodeBlock_triggered();
    void on_action_codeBlock_triggered();
    void on_action_annotation_triggered();
    void on_action_moreAnnotation_triggered();
    void on_action_link_triggered();
    void on_action_linkImage_triggered();
    void on_action_insertImage_triggered();
    void on_action_accessory_triggered();
    void on_action_table_triggered();
    void on_action_orderedList_triggered();
    void on_action_unorderedList_triggered();
    void on_action_quoteBlock_triggered();
    void on_action_cuttingLine_triggered();
    void on_action_nowTime_triggered();

    void on_action_nightMode_triggered();

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
    QAction *getActionIncline() const;
    QAction *getActionStrickout() const;
    QAction *getActionUnderline() const;
    QAction *getActionTable() const;
    QAction *getActionOrderedList() const;
    QAction *getActionUnorderedList() const;
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
    QAction *getActionPrint() const;
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

private:
    Ui::MenuBar *ui;
    QPrinter *mPrinter;
    QActionGroup *mWindowActionGroup;

};

#endif // MENUBAR_H
