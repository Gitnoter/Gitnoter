#ifndef TAGSLISTCELL_H
#define TAGSLISTCELL_H

#include "tagslistcell.h"
#include "tagsmodel.h"

#include <QWidget>

namespace Ui {
class TagsListCell;
}

class TagsListCell : public QWidget
{
    Q_OBJECT

public:
    explicit TagsListCell(QWidget *parent = 0);
    TagsListCell(TagsModel *tagsModel);
    ~TagsListCell();

    TagsModel *gettagsModel() const;
    void settagsModel(TagsModel *tagsModel);

private:
    Ui::TagsListCell *ui;
    TagsModel *mtagsModel;

};

#endif // TagsListCell_H
