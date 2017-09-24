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
    if (categoriesModelList.length() == 0) {
        QMap<QString, CategoriesModel *> map;
        for (auto &&noteModel : noteModelList) {
            if (map.contains(noteModel->categoriesModel->getName())) {
                auto *categoriesModel = map[noteModel->categoriesModel->getName()];
                categoriesModel->setCount(categoriesModel->getCount() + 1);
            }
            else {
                noteModel->categoriesModel->setCount(noteModel->categoriesModel->getCount() + 1);
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

        if (stringList.length() == 0 && categoriesModelList.length() > 0) {
            QString str = "";
            for (auto &&item : categoriesModelList) {
                str += item->getName() + "\n";
            }
            Tools::writerFile(repoCategoriesListPath, str.trimmed());
        }
    }
}

void Global::initTagsModelList()
{
    if (tagsModelList.length() == 0) {
        QMap<QString, TagsModel *> map;
        for (auto &&noteModel : noteModelList) {
            for (auto &&tagsModel : *noteModel->tagsModelList) {
                if (map.contains(tagsModel->getName())) {
                    auto *model = map[tagsModel->getName()];
                    model->setCount(model->getCount() + 1);
                }
                else {
                    tagsModel->setCount(tagsModel->getCount() + 1);
                    map[tagsModel->getName()] = tagsModel;
                }
            }
        }
        tagsModelList = map.values();
    }
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
    configModel->setOpenNotesUuid("c6c71bef-3dbf-4fd4-ab3c-2a111f58fcde5");
    configModel->setSidebarSortKey(1);
    configModel->setSidebarSortValue("DESC");
    configModel->setCategoriesName("test");
    configModel->setIsSelectedClasses(1);

}
