#include "notemodel.h"
#include "tools.h"
#include "globals.h"

NoteModel::NoteModel()
{
    this->clear();
}

NoteModel::NoteModel(CategoriesModel *categoriesModel)
{
    this->clear();
    this->categoriesModel = categoriesModel;
}

NoteModel::NoteModel(QString noteText, QString filePath)
{
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

    while (!in.atEnd()) {
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
    this->contentModel = new ContentModel(map["uuid"], map["title"],
                                          Tools::timestampFromDateTime(map["createDate"]),
                                          Tools::timestampFromDateTime(map["updateDate"]), body.trimmed(),
                                          filePath);
    this->categoriesModel = new CategoriesModel(map["categories"]);
    this->tagsModelList = new QList<TagsModel *>;
    QStringList tags = map["tags"].split(QRegExp(Global::tagSplit + "?"));
    for (auto &&tag : tags) {
        this->tagsModelList->append(new TagsModel(tag.trimmed()));
    }
}

NoteModel::NoteModel(ContentModel *contentModel, QList<TagsModel *> *tagList,
                     CategoriesModel *categoriesModel)
{
    this->contentModel = contentModel;
    this->tagsModelList = tagList;
    this->categoriesModel = categoriesModel;
}

QString NoteModel::getNote() const
{
    QString note;
    QString tags;
    note += "uuid: " + this->contentModel->getUuid() + "\n";
    note += "title: " + this->contentModel->getTitle() + "\n";
    note += "createDate: " + Tools::timestampToDateTime(this->contentModel->getCreateDate()) + "\n";
    note += "updateDate: " + Tools::timestampToDateTime(this->contentModel->getUpdateDate()) + "\n";
    note += "categories: " + this->categoriesModel->getName() + "\n";

    note += "tags: ";
    for (auto &&tagsModel : *(this->tagsModelList)) {
        tags += tagsModel->getName() + Global::tagSplit;
    }
    tags.chop(Global::tagSplit.length());
    note += tags;

    note += "\n\n---\n\n" + this->contentModel->getBody();

    return note;
}

QString NoteModel::getDisplayNote() const
{
    return contentModel->getTitle().isEmpty()
           ? contentModel->getBody()
           : "# " + contentModel->getTitle() + "\n\n" + contentModel->getBody();
}

QString NoteModel::getTagsString() const
{
    QString tagsString;
    for (auto &&item : *Global::openNoteModel->tagsModelList) {
        tagsString += item->getName() + Global::tagSplit;
    }
    tagsString.chop(Global::tagSplit.length());

    return tagsString;
}

void NoteModel::clear()
{
    this->contentModel = new ContentModel();
    this->tagsModelList = new QList<TagsModel *>();
    this->categoriesModel = new CategoriesModel();
}

void NoteModel::writerLocal()
{
    Tools::writerFile(contentModel->getFilePath(), getNote());
}
