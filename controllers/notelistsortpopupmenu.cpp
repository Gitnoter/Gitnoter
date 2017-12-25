#include "notelistsortpopupmenu.h"
#include "globals.h"

NoteListSortPopupMenu::NoteListSortPopupMenu(QPushButton* pushButton, QWidget* parent) :
        QMenu(parent),
        pushButton(pushButton),
        mMainWindow((MainWindow *) parent)
{
    action_title = new QAction(tr("标题"), this);
    action_createDate = new QAction(tr("创建日期"), this);
    action_updateDate = new QAction(tr("更新日期"), this);
    action_asc = new QAction(tr("升序"), this);
    action_desc = new QAction(tr("降序"), this);

    action_title->setObjectName(QStringLiteral("action_title"));
    action_createDate->setObjectName(QStringLiteral("action_createDate"));
    action_updateDate->setObjectName(QStringLiteral("action_updateDate"));
    action_asc->setObjectName(QStringLiteral("action_asc"));
    action_desc->setObjectName(QStringLiteral("action_desc"));

    action_title->setCheckable(true);
    action_createDate->setCheckable(true);
    action_updateDate->setCheckable(true);
    action_asc->setCheckable(true);
    action_desc->setCheckable(true);

    addAction(action_title);
    addAction(action_createDate);
    addAction(action_updateDate);
    addSeparator();
    addAction(action_asc);
    addAction(action_desc);

    connect(action_title, SIGNAL(triggered(bool)), this, SLOT(onActionTitleTriggered(bool)));
    connect(action_createDate, SIGNAL(triggered(bool)), this, SLOT(onActionCreateDateTriggered(bool)));
    connect(action_updateDate, SIGNAL(triggered(bool)), this, SLOT(onActionUpdateDateTriggered(bool)));
    connect(action_asc, SIGNAL(triggered(bool)), this, SLOT(onActionAscTriggered(bool)));
    connect(action_desc, SIGNAL(triggered(bool)), this, SLOT(onActionDescTriggered(bool)));

    Gitnoter::SortBasis basis = gConfigModel->getNoteSortBasis();
    Qt::SortOrder type = gConfigModel->getNoteSortType();
    action_title->setChecked(Gitnoter::Title == basis);
    action_createDate->setChecked(Gitnoter::CreateDate == basis);
    action_updateDate->setChecked(Gitnoter::UpdateDate == basis);
    action_asc->setChecked(Qt::AscendingOrder == type);
    action_desc->setChecked(Qt::DescendingOrder == type);
    setPushButtonText(basis);
}

void NoteListSortPopupMenu::showEvent(QShowEvent* event)
{
    QPoint p = this->pos();
    QRect geo = pushButton->geometry();
    this->move(p.x() +  geo.width() - this->geometry().width(), p.y() + 3);
}

void NoteListSortPopupMenu::onActionTitleTriggered(bool triggered)
{
    setBasisActionChecked(action_title);
    gConfigModel->setNoteSortBasis(Gitnoter::Title);
    setPushButtonText(Gitnoter::Title);
    emit actionTriggered();
}

void NoteListSortPopupMenu::onActionCreateDateTriggered(bool triggered)
{
    setBasisActionChecked(action_createDate);
    gConfigModel->setNoteSortBasis(Gitnoter::CreateDate);
    setPushButtonText(Gitnoter::CreateDate);
    emit actionTriggered();
}

void NoteListSortPopupMenu::onActionUpdateDateTriggered(bool triggered)
{
    setBasisActionChecked(action_updateDate);
    gConfigModel->setNoteSortBasis(Gitnoter::UpdateDate);
    setPushButtonText(Gitnoter::UpdateDate);
    emit actionTriggered();
}

void NoteListSortPopupMenu::onActionAscTriggered(bool triggered)
{
    setTypeActionChecked(action_asc);
    gConfigModel->setNoteSortType(Qt::AscendingOrder);
    emit actionTriggered();
}

void NoteListSortPopupMenu::onActionDescTriggered(bool triggered)
{
    setTypeActionChecked(action_desc);
    gConfigModel->setNoteSortType(Qt::DescendingOrder);
    emit actionTriggered();
}

void NoteListSortPopupMenu::setBasisActionChecked(QAction *action)
{
    action_title->setChecked(false);
    action_createDate->setChecked(false);
    action_updateDate->setChecked(false);

    action->setChecked(true);
}

void NoteListSortPopupMenu::setTypeActionChecked(QAction *action)
{
    action_asc->setChecked(false);
    action_desc->setChecked(false);

    action->setChecked(true);
}

void NoteListSortPopupMenu::setPushButtonText(Gitnoter::SortBasis basis)
{
    QAction *action = nullptr;
    switch (basis) {
        case Gitnoter::Title:action = action_title;break;
        case Gitnoter::CreateDate:action = action_createDate;break;
        case Gitnoter::UpdateDate:action = action_updateDate;break;
        default:break;
    }

    if (action) {
        QString text = tr("  按 %1 排序").arg(action->text());
        pushButton->setText(text);
    }
}

void NoteListSortPopupMenu::setPushButtonText(QAction *action)
{
    QString text = tr("  按 %1 排序").arg(action->text());
    pushButton->setText(text);
}
