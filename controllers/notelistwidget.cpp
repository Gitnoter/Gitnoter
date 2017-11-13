#include "notelistwidget.h"
#include "globals.h"
#include "notelistcellwidget.h"

#include <QDate>
#include <QDir>

NoteListWidget::NoteListWidget(QWidget *parent) : QListWidget(parent)
{

}

NoteListWidget::~NoteListWidget()
{

}

void NoteListWidget::init()
{
    QDir dir(Globals::repoNoteTextPath);
    for (auto &&mfi : dir.entryInfoList()) {
        if (mfi.fileName() == "." || mfi.fileName() == "..") {
            continue;
        }
        if (mfi.isDir()) {
            QDir dir2(mfi.absoluteFilePath());
            NoteModel *noteModel = new NoteModel(dir2.filePath(Globals::noteTextFileName),
                                                 dir2.filePath(Globals::noteDataFileName));
            appendNew(noteModel);
            mNoteModelList.append(noteModel);
        }
    }
}

void NoteListWidget::appendNew(NoteModel *noteModel)
{
    QListWidgetItem *listWidgetItem = new QListWidgetItem(this);
    listWidgetItem->setData(Qt::UserRole, QVariant::fromValue(noteModel));
    addItem(listWidgetItem);
    NoteListCellWidget *noteListCellWidget = new NoteListCellWidget(noteModel, this);
    listWidgetItem->setSizeHint(noteListCellWidget->minimumSize());
    setItemWidget(listWidgetItem, noteListCellWidget);
}

NoteModel *NoteListWidget::appendNew(const QString &category)
{
    NoteModel *noteModel = new NoteModel;
    noteModel->setCategory(category);
    appendNew(noteModel);

    return noteModel;
}

QListWidgetItem *NoteListWidget::getItemByUuid(const QString &uuid)
{
    for (int i = 0; i < count(); ++i) {
        QListWidgetItem *listWidgetItem = item(i);
        if (listWidgetItem->data(Qt::UserRole).value<NoteModel *>()->getUuid() == uuid) {
            return listWidgetItem;
        }
    }

    return nullptr;
}

NoteModel *NoteListWidget::getNoteModelByUuid(const QString &uuid)
{
    QListWidgetItem *listWidgetItem = getItemByUuid(uuid);
    if (listWidgetItem) {
        return listWidgetItem->data(Qt::UserRole).value<NoteModel *>();
    }
    return nullptr;
}

void NoteListWidget::removeOne(NoteModel *noteModel)
{
    QDir(noteModel->getNoteDir()).removeRecursively();
    delete getItemByUuid(noteModel->getUuid());
}

void NoteListWidget::deleteOne(const QString &uuid)
{
    QListWidgetItem *listWidgetItem = getItemByUuid(uuid);
    listWidgetItem->setHidden(true);

    NoteModel *noteModel = listWidgetItem->data(Qt::UserRole).value<NoteModel *>();
    noteModel->setIsDelete(1);
    noteModel->saveNoteDataToLocal();
}

void NoteListWidget::appendOld(const QString &uuid)
{
    QListWidgetItem *listWidgetItem = getItemByUuid(uuid);
    listWidgetItem->setHidden(false);

    NoteModel *noteModel = listWidgetItem->data(Qt::UserRole).value<NoteModel *>();
    noteModel->setIsDelete(0);
    noteModel->saveNoteDataToLocal();
}

void NoteListWidget::showListItems()
{
    Gitnoter::GroupType type = Globals::configModel->getSideSelectedType();
    const QString &name = Globals::configModel->getSideSelectedName();

    for (int i = 0; i < count(); ++i) {
        QListWidgetItem *listWidgetItem = item(i);
        NoteModel *noteModel = listWidgetItem->data(Qt::UserRole).value<NoteModel *>();

        listWidgetItem->setHidden(true);

        if (noteModel->getIsDelete()) {
            if (type == Gitnoter::Trash) {
                listWidgetItem->setHidden(false);
            }
        }
        else if ((type == Gitnoter::All) ||
                 (type == Gitnoter::Recent && noteModel->getUpdateDate() > (QDateTime::currentSecsSinceEpoch() - Globals::sevenDays)) ||
                 (type == Gitnoter::Unclassified && noteModel->getCategory().isEmpty()) ||
                 (type == Gitnoter::Category && name == noteModel->getCategory()) ||
                 (type == Gitnoter::Tag && noteModel->getTagList().indexOf(name) != -1)) {
            listWidgetItem->setHidden(false);
        }
    }
}

const QString NoteListWidget::setItemSelected(const QString &uuid)
{
    clearSelection();
    for (int i = 0; i < count(); ++i) {
        QListWidgetItem *listWidgetItem = item(i);
        if (!listWidgetItem->isHidden()) {
            NoteModel *noteModel = listWidgetItem->data(Qt::UserRole).value<NoteModel *>();
            if (noteModel->getUuid() == uuid) {
                listWidgetItem->setSelected(true);
                return uuid;
            }
        }
    }

    for (int i = 0; i < count(); ++i) {
        QListWidgetItem *listWidgetItem = item(i);
        if (!listWidgetItem->isHidden()) {
            NoteModel *noteModel = listWidgetItem->data(Qt::UserRole).value<NoteModel *>();
            Globals::configModel->setOpenNoteUuid(noteModel->getUuid());
            listWidgetItem->setSelected(true);
            return noteModel->getUuid();
        }
    }

    return "";
}
