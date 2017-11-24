#include "menubar.h"
#include "ui_menubar.h"

#include <qDebug>

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
    QAction *action_about = new QAction(parent);
    action_about->setObjectName(QStringLiteral("action_about"));
    action_about->setText(tr("&关于 Gitnoter"));
    action_about->setMenuRole(QAction::AboutRole);

    QAction *action_preferences = new QAction(parent);
    action_preferences->setObjectName(QStringLiteral("action_preferences"));
    action_preferences->setText(tr("&设置..."));
    action_preferences->setMenuRole(QAction::PreferencesRole);

    //
    // qDebug() << menuBar()->findChild<QMenu *>("menu_file")  // 这里可以查找到
    //      ->findChild<QAction *>("action_preferences");  // 这里死都查找不到, 不知道是什么原因

    QList<QAction *> actionList = menuBar()->findChild<QMenu *>("menu_file")->actions();
    for (auto &&item : actionList) {
        if (item->objectName() == "action_preferences") {
            item->setMenuRole(QAction::PreferencesRole);
            break;
        }
    }

    actionList = menuBar()->findChild<QMenu *>("menu_help")->actions();
    for (auto &&item : actionList) {
        if (item->objectName() == "action_about") {
            item->setMenuRole(QAction::AboutRole);
            break;
        }
    }
}
