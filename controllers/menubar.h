#ifndef MENUBAR_H
#define MENUBAR_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

class MenuBar : public QMenuBar
{
    Q_OBJECT
public:
    explicit MenuBar(QWidget *parent = 0);

private:
    void setupUI(QWidget *parent);

private:
    QAction *action_newNote;
    QAction *action_newCategories;
    QAction *action_newTags;
    QAction *action_saveNote;
    QAction *action_quitNote;
    QAction *action_synch;
    QAction *action_repeal;
    QAction *action_reform;
    QAction *action_cut;
    QAction *action_copy;
    QAction *action_paste;
    QAction *action_selectAll;
    QAction *action_notes;
    QAction *action_noteText;
    QAction *action_toUppercase;
    QAction *action_toLowercase;
    QAction *action_toUppercaseAtFirst;
    QAction *action_deleteNote;
    QAction *action_about;
    QAction *action_preferences;
    QAction *action_feedback;
    QAction *action_issue;
    QAction *action_guide;
    QAction *action_markdownLanguage;
    QAction *action_history;
    QAction *action_prepose;
    QAction *action_maxWindow;
    QAction *action_mixWindow;
    QAction *action_fullScreen;
    QAction *action_editorMode;

    QMenu *menu_file;
    QMenu *menu_edit;
    QMenu *menu_note;
    QMenu *menu_sign;
    QMenu *menu_view;
    QMenu *menu_window;
    QMenu *menu_help;

    QMenu *menu_search;
    QMenu *menu_switch;

};

#endif // MENUBAR_H
