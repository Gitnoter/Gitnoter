#ifndef MENUBAR_H
#define MENUBAR_H

#include "mainwindow.h"
#include "loginwidget.h"
#include "markdowneditorwidget.h"

#include <QMainWindow>
#include <QPrinter>

namespace Ui {
class MenuBar;
}

class MenuBar : public QMainWindow
{
    Q_OBJECT

public:
    explicit MenuBar(QWidget *parent = 0);
    ~MenuBar();

    void setGroupEnable();

//    void initFormMarkdownEditorWidget();
//    void initFormGroupTreeWidget();
//    void initFormNoteListWidget();

private:
    void setupUi(QWidget *parent);

signals:
    void printAccepted(QPrinter *printer);

private slots:
//    void on_action_newCategories_triggered();
//    void on_action_newTags_triggered();
//    void on_action_saveNote_triggered();
    void on_action_synchNote_triggered();
//    void on_action_reloadNotes_triggered();
//    void on_action_findWithFolder_triggered();
    void on_action_importEvernote_triggered();
    void on_action_exportHtmlForPDF_triggered();
    void on_action_exportMarkdownForPDF_triggered();
    void on_action_exportHtml_triggered();
    void on_action_exportMarkdown_triggered();
    void on_action_printPageSetting_triggered();
    void on_action_print_triggered();
//    void on_action_preferences_triggered();

    void onPrintAccepted();
//    void onPageSetupDialogAccepted();

//    void on_action_undo_triggered();
//    void on_action_redo_triggered();
//    void on_action_cut_triggered();
//    void on_action_copy_triggered();
//    void on_action_paste_triggered();
    void on_action_pasteHtml_triggered();
//    void on_action_selectAll_triggered();
//    void on_action_deleteText_triggered();
//    void on_action_deleteNote_triggered();
//    void on_action_copyLine_triggered();
//    void on_action_deleteLine_triggered();
    void on_action_webSearch_triggered();

    void on_action_groupSearch_triggered();
    void on_action_findText_triggered();
    void on_action_findNext_triggered();
    void on_action_findLast_triggered();
    void on_action_replaceText_triggered();
    void on_action_replaceAndNext_triggered();
    void on_action_replaceAll_triggered();

    void on_action_toUppercase_triggered();
    void on_action_toLowercase_triggered();
    void on_action_toUppercaseAtFirst_triggered();

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

    void on_action_sidebar_triggered();
    void on_action_listbar_triggered();
    void on_action_toolbar_triggered();
    void on_action_navigationBar_triggered();
    void on_action_editMode_triggered();
    void on_action_previewMode_triggered();
    void on_action_editPreviewMode_triggered();
    void on_action_nightMode_triggered();
    void on_action_addFontSize_triggered();
    void on_action_subtractFontSize_triggered();
    void on_action_resetFontSize_triggered();
    void on_action_enterFullScreen_triggered();

    void on_action_mixWindow_triggered();
    void on_action_resizeWindow_triggered();
    void on_action_newWindow_triggered();
    void on_action_newEditWindow_triggered();
    void on_action_showLastWindow_triggered();
    void on_action_showNextWindow_triggered();
    void on_action_closeWindow_triggered();
    void on_action_closeAllWindow_triggered();
    void on_action_lock_triggered();
    void on_action_prepose_triggered();

    void on_action_guide_triggered();
    void on_action_history_triggered();
    void on_action_markdownLanguage_triggered();
    void on_action_feedback_triggered();
    void on_action_issue_triggered();
    void on_action_about_triggered();

public:
    Ui::MenuBar *getUi() { return ui; }

private:
    Ui::MenuBar *ui;
    QPrinter *mPrinter;
};

#endif // MENUBAR_H
