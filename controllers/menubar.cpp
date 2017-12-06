#include "ui_menubar.h"
#include "menubar.h"
#include "globals.h"

#include <QPrintDialog>
#include <QPageSetupDialog>

MenuBar::MenuBar(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MenuBar)
{
    ui->setupUi(this);
    setupUi(parent);
}

MenuBar::~MenuBar()
{
    delete ui;
}

void MenuBar::setupUi(QWidget *parent)
{
    ui->action_preferences->setMenuRole(QAction::PreferencesRole);
    ui->action_about->setMenuRole(QAction::AboutRole);

    if (qobject_cast<MainWindow *>(parent)) {
        setGroupEnable();
    }

    if (qobject_cast<LoginWidget *>(parent)) {

    }

    if (qobject_cast<MarkdownEditorWidget *>(parent)) {
        qDebug() << "MarkdownEditorWidget";
    }
}

void MenuBar::setGroupEnable()
{
    Gitnoter::GroupType type = Globals::configModel->getSideSelectedType();
    if (Gitnoter::Category == type) {
        ui->action_newTags->setEnabled(false);
        ui->action_newCategories->setEnabled(true);
    }
    else {
        ui->action_newTags->setEnabled(Gitnoter::Tag == type);
        ui->action_newCategories->setEnabled(false);
    }
}

void MenuBar::on_action_importEvernote_triggered()
{

}

void MenuBar::on_action_exportHtmlForPDF_triggered()
{

}

void MenuBar::on_action_exportMarkdownForPDF_triggered()
{

}

void MenuBar::on_action_exportHtml_triggered()
{

}

void MenuBar::on_action_exportMarkdown_triggered()
{

}

void MenuBar::on_action_printPageSetting_triggered()
{
    mPrinter = new QPrinter();
    QPrintDialog *dialog = new QPrintDialog(mPrinter, parentWidget());
    dialog->open(this, SLOT(onPrintAccepted()));
}

void MenuBar::on_action_print_triggered()
{
    mPrinter = new QPrinter();
    QPageSetupDialog *dialog = new QPageSetupDialog(mPrinter, parentWidget());
    dialog->open(this, SLOT(onPrintAccepted()));
}


void MenuBar::on_action_title1_triggered()
{

}

void MenuBar::on_action_title2_triggered()
{

}

void MenuBar::on_action_title3_triggered()
{

}

void MenuBar::on_action_title4_triggered()
{

}

void MenuBar::on_action_title5_triggered()
{

}

void MenuBar::on_action_strong_triggered()
{

}

void MenuBar::on_action_incline_triggered()
{

}

void MenuBar::on_action_strickout_triggered()
{

}

void MenuBar::on_action_underline_triggered()
{

}

void MenuBar::on_action_innerCodeBlock_triggered()
{

}

void MenuBar::on_action_codeBlock_triggered()
{

}

void MenuBar::on_action_annotation_triggered()
{

}

void MenuBar::on_action_moreAnnotation_triggered()
{

}

void MenuBar::on_action_link_triggered()
{

}

void MenuBar::on_action_linkImage_triggered()
{

}

void MenuBar::on_action_insertImage_triggered()
{

}

void MenuBar::on_action_accessory_triggered()
{

}

void MenuBar::on_action_table_triggered()
{

}

void MenuBar::on_action_orderedList_triggered()
{

}

void MenuBar::on_action_unorderedList_triggered()
{

}

void MenuBar::on_action_quoteBlock_triggered()
{

}

void MenuBar::on_action_cuttingLine_triggered()
{

}

void MenuBar::on_action_nowTime_triggered()
{

}

void MenuBar::on_action_nightMode_triggered()
{

}

void MenuBar::onPrintAccepted()
{
    emit printAccepted(mPrinter);
}

QAction *MenuBar::getActionNewNote() const
{
    return ui->action_newNote;
}

QAction *MenuBar::getActionNewCategories() const
{
    return ui->action_newCategories;
}

QAction *MenuBar::getActionNewTags() const
{
    return ui->action_newTags;
}

QAction *MenuBar::getActionSaveNote() const
{
    return ui->action_saveNote;
}

QAction *MenuBar::getActionSynchNote() const
{
    return ui->action_synchNote;
}

QAction *MenuBar::getActionUndo() const
{
    return ui->action_undo;
}

QAction *MenuBar::getActionRedo() const
{
    return ui->action_redo;
}

QAction *MenuBar::getActionCut() const
{
    return ui->action_cut;
}

QAction *MenuBar::getActionCopy() const
{
    return ui->action_copy;
}

QAction *MenuBar::getActionPaste() const
{
    return ui->action_paste;
}

QAction *MenuBar::getActionSelectAll() const
{
    return ui->action_selectAll;
}

QAction *MenuBar::getActionGroupSearch() const
{
    return ui->action_groupSearch;
}

QAction *MenuBar::getActionFindText() const
{
    return ui->action_findText;
}

QAction *MenuBar::getActionToUppercase() const
{
    return ui->action_toUppercase;
}

QAction *MenuBar::getActionToLowercase() const
{
    return ui->action_toLowercase;
}

QAction *MenuBar::getActionToUppercaseAtFirst() const
{
    return ui->action_toUppercaseAtFirst;
}

QAction *MenuBar::getActionDeleteNote() const
{
    return ui->action_deleteNote;
}

QAction *MenuBar::getActionGuide() const
{
    return ui->action_guide;
}

QAction *MenuBar::getActionMarkdownLanguage() const
{
    return ui->action_markdownLanguage;
}

QAction *MenuBar::getActionFeedback() const
{
    return ui->action_feedback;
}

QAction *MenuBar::getActionHistory() const
{
    return ui->action_history;
}

QAction *MenuBar::getActionIssue() const
{
    return ui->action_issue;
}

QAction *MenuBar::getActionPreposeWindow() const
{
    return ui->action_preposeWindow;
}

QAction *MenuBar::getActionMixWindow() const
{
    return ui->action_mixWindow;
}

QAction *MenuBar::getActionResizeWindow() const
{
    return ui->action_resizeWindow;
}

QAction *MenuBar::getActionEnterFullScreen() const
{
    return ui->action_enterFullScreen;
}

QAction *MenuBar::getActionAbout() const
{
    return ui->action_about;
}

QAction *MenuBar::getActionPreferences() const
{
    return ui->action_preferences;
}

QAction *MenuBar::getActionFindNext() const
{
    return ui->action_findNext;
}

QAction *MenuBar::getActionFindLast() const
{
    return ui->action_findLast;
}

QAction *MenuBar::getActionReplaceText() const
{
    return ui->action_replaceText;
}

QAction *MenuBar::getActionReplaceAndNext() const
{
    return ui->action_replaceAndNext;
}

QAction *MenuBar::getActionReplaceAll() const
{
    return ui->action_replaceAll;
}

QAction *MenuBar::getActionPasteHtml() const
{
    return ui->action_pasteHtml;
}

QAction *MenuBar::getActionWebSearch() const
{
    return ui->action_webSearch;
}

QAction *MenuBar::getActionCopyLine() const
{
    return ui->action_copyLine;
}

QAction *MenuBar::getActionDeleteLine() const
{
    return ui->action_deleteLine;
}

QAction *MenuBar::getActionDeleteText() const
{
    return ui->action_deleteText;
}

QAction *MenuBar::getActionClearText() const
{
    return ui->action_clearText;
}

QAction *MenuBar::getActionTitle1() const
{
    return ui->action_title1;
}

QAction *MenuBar::getActionTitle2() const
{
    return ui->action_title2;
}

QAction *MenuBar::getActionTitle3() const
{
    return ui->action_title3;
}

QAction *MenuBar::getActionTitle4() const
{
    return ui->action_title4;
}

QAction *MenuBar::getActionTitle5() const
{
    return ui->action_title5;
}

QAction *MenuBar::getActionStrong() const
{
    return ui->action_strong;
}

QAction *MenuBar::getActionIncline() const
{
    return ui->action_incline;
}

QAction *MenuBar::getActionStrickout() const
{
    return ui->action_strickout;
}

QAction *MenuBar::getActionUnderline() const
{
    return ui->action_underline;
}

QAction *MenuBar::getActionTable() const
{
    return ui->action_table;
}

QAction *MenuBar::getActionOrderedList() const
{
    return ui->action_orderedList;
}

QAction *MenuBar::getActionUnorderedList() const
{
    return ui->action_unorderedList;
}

QAction *MenuBar::getActionLink() const
{
    return ui->action_link;
}

QAction *MenuBar::getActionLinkImage() const
{
    return ui->action_linkImage;
}

QAction *MenuBar::getActionInsertImage() const
{
    return ui->action_insertImage;
}

QAction *MenuBar::getActionAccessory() const
{
    return ui->action_accessory;
}

QAction *MenuBar::getActionQuoteBlock() const
{
    return ui->action_quoteBlock;
}

QAction *MenuBar::getActionCuttingLine() const
{
    return ui->action_cuttingLine;
}

QAction *MenuBar::getActionNowTime() const
{
    return ui->action_nowTime;
}

QAction *MenuBar::getActionInnerCodeBlock() const
{
    return ui->action_innerCodeBlock;
}

QAction *MenuBar::getActionCodeBlock() const
{
    return ui->action_codeBlock;
}

QAction *MenuBar::getActionAnnotation() const
{
    return ui->action_annotation;
}

QAction *MenuBar::getActionMoreAnnotation() const
{
    return ui->action_moreAnnotation;
}

QAction *MenuBar::getActionNewWindow() const
{
    return ui->action_newWindow;
}

QAction *MenuBar::getActionCloseWindow() const
{
    return ui->action_closeWindow;
}

QAction *MenuBar::getActionCloseAllWindow() const
{
    return ui->action_closeAllWindow;
}

QAction *MenuBar::getActionReloadNotes() const
{
    return ui->action_reloadNotes;
}

QAction *MenuBar::getActionFindWithFolder() const
{
    return ui->action_findWithFolder;
}

QAction *MenuBar::getActionImportEvernote() const
{
    return ui->action_importEvernote;
}

QAction *MenuBar::getActionExportHtmlForPDF() const
{
    return ui->action_exportHtmlForPDF;
}

QAction *MenuBar::getActionExportMarkdownForPDF() const
{
    return ui->action_exportMarkdownForPDF;
}

QAction *MenuBar::getActionExportHtml() const
{
    return ui->action_exportHtml;
}

QAction *MenuBar::getActionExportMarkdown() const
{
    return ui->action_exportMarkdown;
}

QAction *MenuBar::getActionPrintPageSetting() const
{
    return ui->action_printPageSetting;
}

QAction *MenuBar::getActionPrint() const
{
    return ui->action_print;
}

QAction *MenuBar::getActionSidebar() const
{
    return ui->action_sidebar;
}

QAction *MenuBar::getActionListbar() const
{
    return ui->action_listbar;
}

QAction *MenuBar::getActionToolbar() const
{
    return ui->action_toolbar;
}

QAction *MenuBar::getActionNavigationBar() const
{
    return ui->action_navigationBar;
}

QAction *MenuBar::getActionPreviewMode() const
{
    return ui->action_previewMode;
}

QAction *MenuBar::getActionEditMode() const
{
    return ui->action_editMode;
}

QAction *MenuBar::getActionEditPreviewMode() const
{
    return ui->action_editPreviewMode;
}

QAction *MenuBar::getActionFullScreenEditMode() const
{
    return ui->action_fullScreenEditMode;
}

QAction *MenuBar::getActionNightMode() const
{
    return ui->action_nightMode;
}

QAction *MenuBar::getActionPlusFontSize() const
{
    return ui->action_plusFontSize;
}

QAction *MenuBar::getActionSubtractFontSize() const
{
    return ui->action_subtractFontSize;
}

QAction *MenuBar::getActionResetFontSize() const
{
    return ui->action_resetFontSize;
}

QAction *MenuBar::getActionLockWindow() const
{
    return ui->action_lockWindow;
}

QAction *MenuBar::getActionNewEditWindow() const
{
    return ui->action_newEditWindow;
}

QAction *MenuBar::getActionShowLastWindow() const
{
    return ui->action_showLastWindow;
}

QAction *MenuBar::getActionShowNextWindow() const
{
    return ui->action_showNextWindow;
}
