#include "notelistsortpopupmenu.h"

NoteListSortPopupMenu::NoteListSortPopupMenu(QPushButton* pushButton, QWidget* parent) : QMenu(parent), pushButton(pushButton)
{

    QAction *action_createCategories;
    QAction *action_renameCategories;
    QAction *action_removeCategories;
    QAction *action_nameSort;
    QAction *action_countSort;
    QAction *action_timeSort;

    action_createCategories = new QAction("新建文件夹", this);
    action_renameCategories = new QAction("重命名笔记本", this);
    action_removeCategories = new QAction("删除笔记本", this);
    action_nameSort = new QAction("按笔记本名称排序", this);
    action_countSort = new QAction("按笔记数量排序", this);
    action_timeSort = new QAction("按笔记更新时间排序", this);

    action_createCategories->setObjectName(QStringLiteral("action_createCategories"));
    action_renameCategories->setObjectName(QStringLiteral("action_renameCategories"));
    action_removeCategories->setObjectName(QStringLiteral("action_removeCategories"));
    action_nameSort->setObjectName(QStringLiteral("action_nameSort"));
    action_countSort->setObjectName(QStringLiteral("action_countSort"));
    action_timeSort->setObjectName(QStringLiteral("action_timeSort"));

    this->addAction(action_createCategories);
    this->addAction(action_renameCategories);
    this->addAction(action_removeCategories);
    this->addSeparator();
    this->addAction(action_nameSort);
    this->addAction(action_countSort);
    this->addAction(action_timeSort);

//    connect(action_createCategories, &QAction::triggered, this, &MainWindow::on_pushButton_addCategories_clicked);
//    connect(action_renameCategories, &QAction::triggered, this, &MainWindow::onActionRenameCategoriesTriggered);
//    connect(action_removeCategories, &QAction::triggered, this, &MainWindow::on_pushButton_removeCategories_clicked);
//    connect(action_nameSort, &QAction::triggered, this, &MainWindow::onActionNameSortTriggered);
//    connect(action_countSort, &QAction::triggered, this, &MainWindow::onActionCountSortTriggered);
//    connect(action_timeSort, &QAction::triggered, this, &MainWindow::onActionTimeSortTriggered);
}

void NoteListSortPopupMenu::showEvent(QShowEvent* event)
{
    QPoint p = this->pos();
    QRect geo = pushButton->geometry();
    this->move(p.x() +  geo.width() - this->geometry().width(), p.y() + 3);
}
