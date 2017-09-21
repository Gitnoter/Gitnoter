#ifndef TAGSLISTCELL_H
#define TAGSLISTCELL_H

#include "tagslistcell.h"
#include "tagstablemodel.h"

#include <QWidget>

namespace Ui {
class TagsListCell;
}

class TagsListCell : public QWidget
{
    Q_OBJECT

public:
    explicit TagsListCell(QWidget *parent = 0);
    TagsListCell(TagsTableModel *tagsTableModel);
    ~TagsListCell();

    TagsTableModel *getTagsTableModel() const;
    void setTagsTableModel(TagsTableModel *tagsTableModel);

private:
    Ui::TagsListCell *ui;
    TagsTableModel *mTagsTableModel;

};

#endif // TagsListCell_H
