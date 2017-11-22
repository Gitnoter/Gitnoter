#include "menubar.h"

MenuBar::MenuBar(QWidget *parent) :
    QMenuBar(parent)
{
    setupUI(parent);
}

void MenuBar::setupUI(QWidget *parent)
{
    setObjectName(QStringLiteral("menuBar"));

    menu_file = new QMenu(this);
    menu_file->setObjectName(QStringLiteral("menu_file"));
    menu_file->setTitle(tr("文件"));

    menu_edit = new QMenu(this);
    menu_edit->setObjectName(QStringLiteral("menu_edit"));
    menu_edit->setTitle(tr("编辑"));

    menu_note = new QMenu(this);
    menu_note->setObjectName(QStringLiteral("menu_note"));
    menu_note->setTitle(tr("笔记"));

    menu_sign = new QMenu(this);
    menu_sign->setObjectName(QStringLiteral("menu_sign"));
    menu_sign->setTitle(tr("标记"));

    menu_view = new QMenu(this);
    menu_view->setObjectName(QStringLiteral("menu_view"));
    menu_view->setTitle(tr("视图"));

    menu_window = new QMenu(this);
    menu_window->setObjectName(QStringLiteral("menu_window"));
    menu_window->setTitle(tr("窗口"));

    menu_help = new QMenu(this);
    menu_help->setObjectName(QStringLiteral("menu_help"));
    menu_help->setTitle(tr("帮助"));

    menu_search = new QMenu(menu_edit);
    menu_search->setObjectName(QStringLiteral("menu_search"));
    menu_search->setTitle(tr("搜索"));

    menu_switch = new QMenu(menu_edit);
    menu_switch->setObjectName(QStringLiteral("menu_switch"));
    menu_switch->setTitle(tr("转换"));

    addAction(menu_file->menuAction());
    addAction(menu_edit->menuAction());
    addAction(menu_note->menuAction());
    addAction(menu_sign->menuAction());
    addAction(menu_view->menuAction());
    addAction(menu_window->menuAction());
    addAction(menu_help->menuAction());

    action_newNote = new QAction(parent);
    action_newNote->setObjectName(QStringLiteral("action_newNote"));
    action_newNote->setText(tr("新建笔记"));
    action_newNote->setShortcut(tr("Ctrl+N"));

    action_newCategories = new QAction(parent);
    action_newCategories->setObjectName(QStringLiteral("action_newCategories"));
    action_newCategories->setText(tr("新建笔记本"));
    action_newCategories->setShortcut(tr("Meta+Ctrl+N"));

    action_newTags = new QAction(parent);
    action_newTags->setObjectName(QStringLiteral("action_newTags"));
    action_newTags->setText(tr("新建标签"));
    action_newTags->setShortcut(tr("Meta+Ctrl+T"));

    action_saveNote = new QAction(parent);
    action_saveNote->setObjectName(QStringLiteral("action_saveNote"));
    action_saveNote->setText(tr("保存"));
    action_saveNote->setShortcut(tr("Ctrl+S"));

    action_quitNote = new QAction(parent);
    action_quitNote->setObjectName(QStringLiteral("action_quitNote"));
    action_quitNote->setText(tr("关闭"));
    action_quitNote->setShortcut(tr("Ctrl+W"));

    action_synch = new QAction(parent);
    action_synch->setObjectName(QStringLiteral("action_synch"));
    action_synch->setText(tr("同步笔记"));
    action_synch->setShortcut(tr("Meta+Ctrl+S"));

    action_repeal = new QAction(parent);
    action_repeal->setObjectName(QStringLiteral("action_repeal"));
    action_repeal->setText(tr("撤销"));
    action_repeal->setShortcut(tr("Ctrl+Z"));

    action_reform = new QAction(parent);
    action_reform->setObjectName(QStringLiteral("action_reform"));
    action_reform->setText(tr("重做"));
    action_reform->setShortcut(tr("Ctrl+Shift+Z"));

    action_cut = new QAction(parent);
    action_cut->setObjectName(QStringLiteral("action_cut"));
    action_cut->setText(tr("剪切"));
    action_cut->setShortcut(tr("Ctrl+X"));

    action_copy = new QAction(parent);
    action_copy->setObjectName(QStringLiteral("action_copy"));
    action_copy->setText(tr("复制"));
    action_copy->setShortcut(tr("Ctrl+C"));

    action_paste = new QAction(parent);
    action_paste->setObjectName(QStringLiteral("action_paste"));
    action_paste->setText(tr("粘贴"));
    action_paste->setShortcut(tr("Ctrl+V"));

    action_selectAll = new QAction(parent);
    action_selectAll->setObjectName(QStringLiteral("action_selectAll"));
    action_selectAll->setText(tr("全选"));
    action_selectAll->setShortcut(tr("Ctrl+A"));

    action_notes = new QAction(parent);
    action_notes->setObjectName(QStringLiteral("action_notes"));
    action_notes->setText(tr("搜索笔记"));
    action_notes->setShortcut(tr("Ctrl+Alt+F"));

    action_noteText = new QAction(parent);
    action_noteText->setObjectName(QStringLiteral("action_noteText"));
    action_noteText->setText(tr("在笔记中搜索"));
    action_noteText->setShortcut(tr("Ctrl+F"));

    action_toUppercase = new QAction(parent);
    action_toUppercase->setObjectName(QStringLiteral("action_toUppercase"));
    action_toUppercase->setText(tr("使用大写字母"));

    action_toLowercase = new QAction(parent);
    action_toLowercase->setObjectName(QStringLiteral("action_toLowercase"));
    action_toLowercase->setText(tr("使用小写字母"));

    action_toUppercaseAtFirst = new QAction(parent);
    action_toUppercaseAtFirst->setObjectName(QStringLiteral("action_toUppercaseAtFirst"));
    action_toUppercaseAtFirst->setText(tr("开头字母大写"));

    action_deleteNote = new QAction(parent);
    action_deleteNote->setObjectName(QStringLiteral("action_deleteNote"));
    action_deleteNote->setText(tr("删除笔记"));

    action_feedback = new QAction(parent);
    action_feedback->setObjectName(QStringLiteral("action_feedback"));
    action_feedback->setText(tr("意见反馈..."));

    action_guide = new QAction(parent);
    action_guide->setObjectName(QStringLiteral("action_guide"));
    action_guide->setText(tr("使用指南..."));

    action_markdownLanguage = new QAction(parent);
    action_markdownLanguage->setObjectName(QStringLiteral("action_markdownLanguage"));
    action_markdownLanguage->setText(tr("markdown 语法..."));

    action_history = new QAction(parent);
    action_history->setObjectName(QStringLiteral("action_history"));
    action_history->setText(tr("更新记录..."));

    action_issue = new QAction(parent);
    action_issue->setObjectName(QStringLiteral("action_issue"));
    action_issue->setText(tr("GitHub issues..."));

    action_prepose = new QAction(parent);
    action_prepose->setObjectName(QStringLiteral("action_prepose"));
    action_prepose->setText(tr("前置所有窗口"));

    action_mixWindow = new QAction(parent);
    action_mixWindow->setObjectName(QStringLiteral("action_mixWindow"));
    action_mixWindow->setText(tr("最小化"));
    action_mixWindow->setShortcut(tr("Ctrl+M"));

    action_maxWindow = new QAction(parent);
    action_maxWindow->setObjectName(QStringLiteral("action_maxWindow"));
    action_maxWindow->setText(tr("最大化"));

    action_fullScreen = new QAction(parent);
    action_fullScreen->setObjectName(QStringLiteral("action_fullScreen"));
    action_fullScreen->setText(tr("进入全屏幕"));

    action_editorMode = new QAction(parent);
    action_editorMode->setObjectName(QStringLiteral("action_editorMode"));
    action_editorMode->setText(tr("编辑模式"));

    action_about = new QAction(parent);
    action_about->setObjectName(QStringLiteral("action_about"));
    action_about->setText(tr("&关于 Gitnoter"));
    action_about->setMenuRole(QAction::AboutRole);

    action_preferences = new QAction(parent);
    action_preferences->setObjectName(QStringLiteral("action_preferences"));
    action_preferences->setText(tr("&Preferences..."));
    action_preferences->setMenuRole(QAction::PreferencesRole);

    menu_file->addAction(action_newNote);
    menu_file->addAction(action_newCategories);
    menu_file->addAction(action_newTags);
    menu_file->addSeparator();
    menu_file->addAction(action_saveNote);
    menu_file->addAction(action_quitNote);
    menu_file->addSeparator();
    menu_file->addAction(action_synch);
    menu_file->addAction(action_preferences);

    menu_edit->addAction(action_repeal);
    menu_edit->addAction(action_reform);
    menu_edit->addSeparator();
    menu_edit->addAction(action_cut);
    menu_edit->addAction(action_copy);
    menu_edit->addAction(action_paste);
    menu_edit->addAction(action_selectAll);
    menu_edit->addSeparator();
    menu_edit->addAction(menu_search->menuAction());
    menu_edit->addAction(menu_switch->menuAction());

    menu_note->addAction(action_deleteNote);

    menu_sign->addAction(action_deleteNote);

    menu_view->addAction(action_editorMode);
    menu_view->addSeparator();
    menu_view->addAction(action_fullScreen);

    menu_window->addAction(action_mixWindow);
    menu_window->addAction(action_maxWindow);
    menu_window->addSeparator();
    menu_window->addAction(action_prepose);
    menu_window->addSeparator();

    menu_help->addAction(action_guide);
    menu_help->addAction(action_history);
    menu_help->addAction(action_markdownLanguage);
    menu_help->addSeparator();
    menu_help->addAction(action_feedback);
    menu_help->addAction(action_issue);
    menu_help->addAction(action_about);

    menu_search->addAction(action_notes);
    menu_search->addAction(action_noteText);

    menu_switch->addAction(action_toUppercase);
    menu_switch->addAction(action_toLowercase);
    menu_switch->addAction(action_toUppercaseAtFirst);

}

