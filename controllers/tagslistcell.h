#ifndef TAGSLISTCELL_H
#define TAGSLISTCELL_H

#include "tagslistcell.h"
#include "tagtablemodel.h"

#include <QWidget>

namespace Ui {
class TagsListCell;
}

class TagsListCell : public QWidget
{
    Q_OBJECT

public:
    explicit TagsListCell(QWidget *parent = 0);
    TagsListCell(TagTableModel *tagTableModel);
    ~TagsListCell();

    TagTableModel *getTagTableModel() const;
    void setTagTableModel(TagTableModel *tagTableModel);

private:
    Ui::TagsListCell *ui;
    TagTableModel *m_tagTableModel;

};

#endif // TagsListCell_H
