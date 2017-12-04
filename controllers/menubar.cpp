#include "ui_menubar.h"
#include "menubar.h"
#include "globals.h"

#include <QPrintDialog>
#include <QPageSetupDialog>
#include <QPrinter>

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

void MenuBar::on_action_guide_triggered()
{

}

void MenuBar::on_action_markdownLanguage_triggered()
{

}

void MenuBar::on_action_feedback_triggered()
{

}

void MenuBar::on_action_history_triggered()
{

}

void MenuBar::on_action_issue_triggered()
{

}

void MenuBar::on_action_prepose_triggered()
{

}

void MenuBar::on_action_mixWindow_triggered()
{

}

void MenuBar::on_action_about_triggered()
{

}


void MenuBar::on_action_synchNote_triggered()
{

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

void MenuBar::on_action_resizeWindow_triggered()
{

}

void MenuBar::on_action_newWindow_triggered()
{

}

void MenuBar::on_action_newEditWindow_triggered()
{

}

void MenuBar::on_action_showLastWindow_triggered()
{

}

void MenuBar::on_action_showNextWindow_triggered()
{

}

void MenuBar::on_action_closeWindow_triggered()
{

}

void MenuBar::on_action_closeAllWindow_triggered()
{

}

void MenuBar::on_action_lock_triggered()
{

}

void MenuBar::onPrintAccepted()
{
    emit printAccepted(mPrinter);
}
