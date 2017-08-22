#include "notemodel.h"
#include "tools.h"
#include "appconfig.h"

NoteModel::NoteModel(QString noteText) {
    QTextStream in(&noteText);
    QString uuid;
    QString title;
    int createDate = 0;
    int updateDate = 0;
    QString body;
    QStringList tags;
    QString categories;

    int i = 0;
    while(!in.atEnd()) {
        QString noteLine = in.readLine();

        if (i == 0) {
            uuid = noteLine.mid(6);
        }
        else if (i == 1) {
            title = noteLine.replace("title: ", "");
        }
        else if (i == 2) {
            createDate = Tools::timestampFromDateTime(noteLine.replace("createDate: ", ""));
        }
        else if (i == 3) {
            updateDate = Tools::timestampFromDateTime(noteLine.replace("updateDate: ", ""));
        }
        else if (i == 4) {
            tags = noteLine.replace("title: ", "").split(AppConfig::tagSplit);
        }
        else if (i == 5) {
            categories = noteLine.replace("categories: ", "");
        }
        else if (i == 6 || i == 7 || i == 8) {

        }
        else {
            body += noteLine + "\n";
        }
        i += 1;
    }
    body.chop(2);
    this->noteTableModel = new NoteTableModel(uuid, title, createDate, updateDate, body);
    this->categoriseTableModel = new CategoriseTableModel(categories);
    this->tagTableList = new QList<TagTableModel *>;
    for (auto &&tag : tags) {
        this->tagTableList->append(new TagTableModel(tag));
    }
}

NoteModel::NoteModel(NoteTableModel *noteTableModel, QList<TagTableModel *> *tagTableList,
                     CategoriseTableModel *categoriseTableModel) {
    this->noteTableModel = noteTableModel;
    this->tagTableList = tagTableList;
    this->categoriseTableModel = categoriseTableModel;
}

QString NoteModel::getNote()
{
    QString note;
    note += "title: " + this->noteTableModel->getTitle() + "\n";
    note += "createDate: " + Tools::timestampToDateTime(this->noteTableModel->getCreateDate()) + "\n";
    note += "updateDate" + Tools::timestampToDateTime(this->noteTableModel->getUpdateDate()) + "\n";
    note += "categories: " + this->categoriseTableModel->getName() + "\n";

    note += "notes: ";
    for (auto &&tagTableModel : *(this->tagTableList)) {
        note += tagTableModel->getName() + "\n";
    }
    note.chop(2);

    note += "\n\n---\n\n" + this->noteTableModel->getBody();

    return note;
}
