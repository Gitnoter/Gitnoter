#include "notemodel.h"
#include "tools.h"
#include "globals.h"

NoteModel::NoteModel()
{
    this->clear();
}

NoteModel::NoteModel(QString noteText, QString filePath) {
    QTextStream in(&noteText);
    bool bodyStart = false;
    QString body;
    QMap<QString, QString> map;
    map["uuid"] = "";
    map["title"] = "";
    map["createDate"] = "";
    map["updateDate"] = "";
    map["tags"] = "";
    map["categories"] = "";

    while(!in.atEnd()) {
        QString noteLine = in.readLine().trimmed();

        if (!bodyStart) {
            if (noteLine.mid(0, 1) == "#" || noteLine.mid(0, 2) == "##" || noteLine.mid(0, 3) == "###") {
                bodyStart = true;
            }
            if (noteLine == "---") {
                bodyStart = true;
                continue;
            }
        }

        if (!bodyStart && noteLine.isEmpty()) {
            continue;
        }

        if (!bodyStart) {
            QStringList stringList = Tools::splitNotesData(noteLine);
            if (stringList.length() == 2) {
                map[stringList[0]] = stringList[1].trimmed();
            }
        }
        else {
            body += noteLine + "\n";
        }
    }
    this->noteTableModel = new NoteTableModel(map["uuid"], map["title"], Tools::timestampFromDateTime(map["createDate"])
            , Tools::timestampFromDateTime(map["updateDate"]), body.trimmed(), filePath);
    this->categoriesTableModel = new CategoriseTableModel(map["categories"]);
    this->tagTableList = new QList<TagTableModel *>;
    QStringList tags = map["tags"].split(QRegExp(g_tagSplit + "?"));
    for (auto &&tag : tags) {
        this->tagTableList->append(new TagTableModel(tag.trimmed()));
    }
}

NoteModel::NoteModel(NoteTableModel *noteTableModel, QList<TagTableModel *> *tagTableList,
                     CategoriseTableModel *categoriesTableModel) {
    this->noteTableModel = noteTableModel;
    this->tagTableList = tagTableList;
    this->categoriesTableModel = categoriesTableModel;
}

QString NoteModel::getNote()
{
    QString note;
    note += "uuid: " + this->noteTableModel->getUuid() + "\n";
    note += "title: " + this->noteTableModel->getTitle() + "\n";
    note += "createDate: " + Tools::timestampToDateTime(this->noteTableModel->getCreateDate()) + "\n";
    note += "updateDate: " + Tools::timestampToDateTime(this->noteTableModel->getUpdateDate()) + "\n";
    note += "categories: " + this->categoriesTableModel->getName() + "\n";

    note += "tags: ";
    for (auto &&tagTableModel : *(this->tagTableList)) {
        note += tagTableModel->getName() + g_tagSplit;
    }
    note.chop(g_tagSplit.length());

    note += "\n\n---\n\n" + this->noteTableModel->getBody();

    return note;
}

void NoteModel::clear()
{
    this->noteTableModel = new NoteTableModel();
    this->tagTableList = new QList<TagTableModel *>();
    this->categoriesTableModel = new CategoriseTableModel();
}
