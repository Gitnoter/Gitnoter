#include "ui_categorylistwidget.h"

#include "categorylistwidget.h"
#include "tools.h"

CategoryListWidget::CategoryListWidget(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::CategoryListWidget),
        newGroupModel(nullptr)
{
    ui->setupUi(this);
    mMainWindow = ((MarkdownEditorWidget *) parent)->mainWindow();
    ui->listWidget_data->setAttribute(Qt::WA_MacShowFocusRect, false);
    ui->lineEdit->addAction(QIcon(":/images/icon-search.png"), QLineEdit::LeadingPosition);
}

CategoryListWidget::~CategoryListWidget()
{
    delete ui;
}

void CategoryListWidget::init(NoteModel *noteModel, MarkdownEditorWidget *markdownEditorWidget)
{
    mMarkdownEditorWidget = markdownEditorWidget;
    if (noteModel->getCategory().isEmpty()) {
        oldGroupModel = mMarkdownEditorWidget->mainWindow()->groupTreeWidget()->getGroupModel(Gitnoter::Unclassified);
    }
    else {
        oldGroupModel = mMarkdownEditorWidget->mainWindow()->groupTreeWidget()->getGroupModel(
                Gitnoter::Category, noteModel->getCategory());
    }
    setCategoryList(oldGroupModel);
    ui->lineEdit->clear();
    ui->pushButton_addCategory->setHidden(true);
}

void CategoryListWidget::append()
{
    const QString text = ui->lineEdit->text();
    if (!text.isEmpty()) {
        mMarkdownEditorWidget->appendCategory(text);
        setCategoryList(oldGroupModel);
        search();
        ui->listWidget_data->sortItems(Qt::AscendingOrder);
    }
}

void CategoryListWidget::change()
{
    oldGroupModel->setCount(oldGroupModel->getCount() - 1);
    newGroupModel->setCount(newGroupModel->getCount() + 1);
    mMarkdownEditorWidget->changeCategory(newGroupModel->getName());
}

void CategoryListWidget::search()
{
    const QString text = ui->lineEdit->text();
    int showCount = 0;
    for (int i = 0; i < ui->listWidget_data->count(); ++i) {
        QListWidgetItem *listWidgetItem = ui->listWidget_data->item(i);
        if (listWidgetItem->text().indexOf(text, 0, Qt::CaseInsensitive) == -1) {
            listWidgetItem->setHidden(true);
        }
        else {
            listWidgetItem->setHidden(false);
            showCount += 1;
        }
    }

    ui->pushButton_addCategory->setHidden(!(!text.isEmpty() && showCount == 0));
}

void CategoryListWidget::setCategoryList(GroupModel *groupModel)
{
    QList<GroupModel *> groupModelList = mMarkdownEditorWidget->mainWindow()->groupTreeWidget()
            ->getGroupModelList(Gitnoter::Category);
    ui->listWidget_data->clear();
    for (int i = 0; i < groupModelList.length(); ++i) {
        if (groupModelList[i]->getName().isEmpty()) {
            continue;
        }
        QListWidgetItem *listWidgetItem = new QListWidgetItem(groupModelList[i]->getName());
        listWidgetItem->setData(Qt::UserRole, QVariant::fromValue(groupModelList[i]));
        ui->listWidget_data->addItem(listWidgetItem);
        if (groupModel->getName() == groupModelList[i]->getName()) {
            ui->listWidget_data->setItemSelected(listWidgetItem, true);
        }
    }
}

void CategoryListWidget::on_listWidget_data_doubleClicked(const QModelIndex &)
{
    on_buttonBox_accepted();
    emit accept();
}

void CategoryListWidget::on_pushButton_addCategory_clicked()
{
    append();
}

void CategoryListWidget::on_lineEdit_textChanged(const QString &)
{
    search();
}

void CategoryListWidget::on_listWidget_data_itemClicked(QListWidgetItem *item)
{
    newGroupModel = item->data(Qt::UserRole).value<GroupModel *>();
}

void CategoryListWidget::on_buttonBox_accepted()
{
    if (newGroupModel && oldGroupModel != newGroupModel) {
        change();
    }
}

void CategoryListWidget::showEvent(QShowEvent *showEvent)
{
    QDialog::showEvent(showEvent);

    mMainWindow->gAnalytics()->sendScreenView(objectName());
}
