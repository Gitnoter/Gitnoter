#include "globals.h"
#include "tools.h"

const QString Global::packageName = "com.huyaohui.gitnoter";
const QString Global::version = "1.0.0";
const QString Global::dateFormat = "yyyy/MM/dd hh:mm:ss";
const QString Global::tagSplit = ", ";

const QString Global::appDataLocation = QDir(
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).filePath(
        packageName);
const QString Global::appDataPath = QDir(appDataLocation).filePath("data");
const QString Global::appConfigPath = QDir(appDataPath).filePath("appconfig");
const QString Global::repoPath = QDir(appDataLocation).filePath("user/user.git");
const QString Global::repoNotePath = QDir(repoPath).filePath("notes");
const QString Global::repoResourcePath = QDir(repoPath).filePath("resources");
const QString Global::repoDataPath = QDir(repoPath).filePath("data");
const QString Global::repoCategoriesListPath = QDir(repoDataPath).filePath("categories.list");
const QString Global::repoTagsListPath = QDir(repoDataPath).filePath("tags.list");

QList<NoteModel *> Global::noteModelList = {};
QList<CategoriesModel *> Global::categoriesModelList = {};
QList<TagsModel *> Global::tagsModelList = {};

NoteModel *Global::openNoteModel = new NoteModel();
ConfigModel *Global::configModel = new ConfigModel();
GitManager *Global::gitManager = new GitManager();

void Global::initNoteModelList()
{
    if (noteModelList.length() == 0) {
        QFileInfoList fileInfoList = Tools::getFilesPath(repoNotePath);
        noteModelList.clear();
        for (auto &&fileInfo : fileInfoList) {
            NoteModel *noteModel = new NoteModel(Tools::readerFile(fileInfo.absoluteFilePath()),
                                                 fileInfo.absoluteFilePath());
            noteModelList.append(noteModel);
        }
    }
}

void Global::initCategoriesModelList()
{
    categoriesModelList.clear();

    QMap<QString, CategoriesModel *> map;
    for (auto &&noteModel : noteModelList) {
        if (map.contains(noteModel->categoriesModel->getName())) {
            auto *categoriesModel = map[noteModel->categoriesModel->getName()];
            categoriesModel->setCount(categoriesModel->getCount() + 1);
        }
        else {
            noteModel->categoriesModel->setCount(1);
            map[noteModel->categoriesModel->getName()] = noteModel->categoriesModel;
        }
    }

    QStringList stringList = Tools::readerFileToList(repoCategoriesListPath);
    for (auto &&str : stringList) {
        if (!str.isEmpty() && !map.contains(str)) {
            map[str] = new CategoriesModel(str);
        }
    }

    categoriesModelList = map.values();

    if (stringList.length() == 0) {
        saveCategoriesModelList();
    }
}

bool Global::addCategoriesModelList(QList<CategoriesModel *> list)
{
    bool isWriterFile = false;
    for (auto &&item : list) {
        bool hasName = false;
        for (auto &&categoriesModel : categoriesModelList) {
            if (categoriesModel->getName() == item->getName()) {
                hasName = true;
            }
        }
        if (!hasName) {
            isWriterFile = true;
            categoriesModelList.append(item);
        }
    }

    if (isWriterFile || list.length() == 0) {
        saveCategoriesModelList();
    }

    return isWriterFile;
}

bool Global::hasInCategoriesModelList(const QString &name)
{
    for (auto &&item : categoriesModelList) {
        if (item->getName() == name) {
            return true;
        }
    }

    return false;
}

void Global::initTagsModelList()
{
    tagsModelList.clear();

    QMap<QString, TagsModel *> map;
    for (auto &&noteModel : noteModelList) {
        for (auto &&tagsModel : *noteModel->tagsModelList) {
            if (tagsModel->getName().isEmpty()) {
                continue;
            }
            if (map.contains(tagsModel->getName())) {
                auto *model = map[tagsModel->getName()];
                model->setCount(model->getCount() + 1);
            }
            else {
                tagsModel->setCount(1);
                map[tagsModel->getName()] = tagsModel;
            }
        }
    }

    QStringList stringList = Tools::readerFileToList(repoTagsListPath);
    for (auto &&str : stringList) {
        if (!str.isEmpty() && !map.contains(str)) {
            map[str] = new TagsModel(str);
        }
    }

    tagsModelList = map.values();

    if (stringList.length() == 0) {
        saveTagsModelList();
    }
}

bool Global::addTagsModelList(QList<TagsModel *> list)
{
    bool isWriterFile = false;
    for (auto &&item : list) {
        bool hasName = false;
        for (auto &&tagsModel : tagsModelList) {
            if (tagsModel->getName() == item->getName()) {
                hasName = true;
            }
        }
        if (!hasName) {
            isWriterFile = true;
            tagsModelList.append(item);
        }
    }

    if (isWriterFile || list.length() == 0) {
        saveTagsModelList();
    }

    return isWriterFile;
}

NoteModel *Global::getNoteModelByUuid(const QString &uuid)
{
    for (auto &&noteModel :noteModelList) {
        if (noteModel->contentModel->getUuid() == uuid) {
            return noteModel;
        }
    }

    return nullptr;
}

void Global::initConfigModel()
{
    configModel->unserialize(Tools::readerFile(appConfigPath));

    configModel->setOpenNotesUuid("c6c71bef-3dbf-4fd4-ab3c-2a111f58fcde5");
    configModel->setSidebarSortKey(1);
    configModel->setSidebarSortValue("DESC");
    configModel->setCategoriesName("test");
    configModel->setIsSelectedClasses(1);
}

void Global::setConfigModel()
{
    Tools::writerFile(appConfigPath, configModel->serialize());
}

void Global::setOpenNoteModel(NoteModel *noteModel)
{
    openNoteModel = noteModel;
    configModel->setOpenNotesUuid(noteModel->contentModel->getUuid());

    addNoteModelToList(noteModel);
}

void Global::addNoteModelToList(NoteModel *noteModel)
{
    if (noteModelList.indexOf(noteModel) == -1) {
        noteModelList.append(noteModel);
        initCategoriesModelList();
        initTagsModelList();
    }
}

bool Global::hasInTagsModelList(const QString &name)
{
    for (auto &&item : tagsModelList) {
        if (item->getName() == name) {
            return true;
        }
    }

    return false;
}

void Global::saveCategoriesModelList()
{
    QString str = "";
    for (auto &&item : categoriesModelList) {
        str += item->getName() + "\n";
    }
    Tools::writerFile(repoCategoriesListPath, str.trimmed());
}

void Global::saveTagsModelList()
{
    QString str = "";
    for (auto &&item : tagsModelList) {
        str += item->getName() + "\n";
    }
    Tools::writerFile(repoTagsListPath, str.trimmed());
}

void Global::renameTagsModelListInName(const QString oldName, const QString newName)
{
    for (auto &&item : noteModelList) {
        for (auto &&tagsModel : *item->tagsModelList) {
            if (tagsModel->getName() == oldName) {
                tagsModel->setName(newName);
                item->writerLocal();
                break;
            }
        }
    }

    saveTagsModelList();
}

void Global::renameCategoriesModelListInName(const QString oldName, const QString newName)
{
    for (auto &&item : noteModelList) {
        if (item->categoriesModel->getName() == oldName) {
            item->categoriesModel->setName(newName);
            item->writerLocal();
        }
    }

    saveCategoriesModelList();
}
