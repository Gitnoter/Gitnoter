#include "globals.h"
#include "tools.h"

QString gPackageName = "com.huyaohui.gitnoter";
QString gVersion = "1.0.0";
QString gDateFormat = "yyyy/MM/dd hh:mm:ss";
QString gTagSplit = ", ";
QString gAppDataLocation = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).filePath(
        gPackageName);
QString gDbPath = QDir(gAppDataLocation).filePath("db/");
QString gDbName = "gitnoter.db";
QString gRepoPath = QDir(gAppDataLocation).filePath("user/user.git");
QString gNoteFolderName = "notes";
QString gAudiosFolderName = "files/audios";
QString gImagesFolderName = "files/images";
QString gVideosFolderName = "files/videos";

QList<NoteModel *> gNoteModelList = {};
QList<CategoriesModel *> gCategoriesModelList = {};
QList<TagsModel *> gTagsModelList = {};
NoteModel *gOpenNoteModel = new NoteModel();
ConfigModel *gConfigModel = new ConfigModel();
GitManager *gGitManager = new GitManager();

void gInitNoteModelList()
{
    if (gNoteModelList.length() == 0) {
        QString notesPath = QDir(gRepoPath).filePath(gNoteFolderName);
        QFileInfoList fileInfoList = Tools::getFilesPath(notesPath);
        gNoteModelList.clear();
        for (auto &&fileInfo : fileInfoList) {
            NoteModel *noteModel = new NoteModel(Tools::readerFile(fileInfo.absoluteFilePath()),
                                                 fileInfo.absoluteFilePath());
            gNoteModelList.append(noteModel);
        }
    }
}

void gInitCategoriesModelList()
{
    if (gCategoriesModelList.length() == 0) {
        QMap<QString, CategoriesModel *> map;
        for (auto &&noteModel : gNoteModelList) {
            if (map.contains(noteModel->categoriesModel->getName())) {
                auto *categoriesModel = map[noteModel->categoriesModel->getName()];
                categoriesModel->setCount(categoriesModel->getCount() + 1);
            }
            else {
                noteModel->categoriesModel->setCount(noteModel->categoriesModel->getCount() + 1);
                map[noteModel->categoriesModel->getName()] = noteModel->categoriesModel;
            }
        }
        gCategoriesModelList = map.values();
    }
}

void gInitTagsModelList()
{
    if (gTagsModelList.length() == 0) {
        QMap<QString, TagsModel *> map;
        for (auto &&noteModel : gNoteModelList) {
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
        gTagsModelList = map.values();
    }
}

NoteModel *gGetNoteModelByUuid(const QString &uuid)
{
    for (auto &&noteModel :gNoteModelList) {
        if (noteModel->contentModel->getUuid() == uuid) {
            return noteModel;
        }
    }

    return nullptr;
}

void gInitConfigModel()
{
    gConfigModel->setOpenNotesUuid("c6c71bef-3dbf-4fd4-ab3c-2a111f58fcde5");
    gConfigModel->setSidebarSortKey(1);
    gConfigModel->setSidebarSortValue("DESC");
    gConfigModel->setCategoriesName("test");
    gConfigModel->setIsSelectedClasses(1);

}