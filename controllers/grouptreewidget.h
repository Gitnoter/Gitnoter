#ifndef GROUPTREEWIDGET_H
#define GROUPTREEWIDGET_H

#include "groupmodel.h"
#include "mainwindow.h"

#include <QTreeWidget>

class MainWindow;

class GroupTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    explicit GroupTreeWidget(QWidget *parent = 0);

    ~GroupTreeWidget();

    void init(QList<NoteModel *> noteModelList, MainWindow *mainWindow);

    void remove(Gitnoter::GroupType type, const QString &name);
    bool append(Gitnoter::GroupType type, const QString &text, int num = 0);
    void modify(GroupModel *groupModel, const QString &oldText, const QString &newText);
    void subtract(Gitnoter::GroupType type, const QString &text = "", int num = 1);
    void subtract(NoteModel *noteModel, bool remove = false);
    void add(Gitnoter::GroupType type, const QString &text = "", int num = 1);
    void add(NoteModel *noteModel);
    void setItemSelected();

public slots:
    void onItemChanged(QTreeWidgetItem *item, int column);

private:
    GroupModel *append(GroupModel *groupModel);

    void removeOne(GroupModel *groupModel);

    void removeOne(Gitnoter::GroupType type, const QString &text);

    void appendAny(GroupModel *groupModel, int num);

    void appendOne(NoteModel *noteModel, int num = 1);

    bool has(Gitnoter::GroupType type, const QString &text);

    QString toString(Gitnoter::GroupType type);

    void saveDataToLocal(Gitnoter::GroupType type);

    void appendToGroupModelMap(QMap<QString, GroupModel *> &map, Gitnoter::GroupType type, QString name, int count);

public:
    MainWindow *mainWindow() { return mMainWindow; }

    int count() { return mCount; }

    QTreeWidgetItem *getTreeWidgetItem(Gitnoter::GroupType type, const QString &text = "");

    QTreeWidgetItem *getTreeWidgetItem(GroupModel &groupModel);

    QList<GroupModel *> getGroupModelList();

    QList<GroupModel *> getGroupModelList(Gitnoter::GroupType type);

    GroupModel *getGroupModel(Gitnoter::GroupType type, const QString &text = "");

    GroupModel *getGroupModel(QTreeWidgetItem *treeWidgetItem);

    GroupModel *selectedGroupModel();

private:
    MainWindow *mMainWindow;
    int mCount;

};

#endif // GROUPTREEWIDGET_H
