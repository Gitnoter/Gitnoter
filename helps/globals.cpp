#include "globals.h"
#include "tools.h"

const QString Global::packageName = "com.huyaohui.gitnoter";
const QString Global::version = "1.0.0";
const QString Global::dateFormat = "yyyy/MM/dd hh:mm";
const QString Global::tagSplit = ", ";

const QString Global::appDataLocation = QDir(
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).filePath(
        packageName);
const QString Global::appDataPath = QDir(appDataLocation).filePath("data/");
const QString Global::appConfigPath = QDir(appDataPath).filePath("appconfig.json");
const QString Global::repoPath = QDir(appDataLocation).filePath("user/user.git");
const QString Global::repoNotePath = QDir(repoPath).filePath("notes/");
const QString Global::noteTextFileName = "markdown.md";
const QString Global::noteDataFileName = "data.json";
const QString Global::repoCategoriesListPath = QDir(noteDataFileName).filePath("categories.list");
const QString Global::repoTagsListPath = QDir(noteDataFileName).filePath("tags.list");

const QString Global::newNoteKeySequence = "Ctrl+N";
const QString Global::lockWindowKeySequence = "Ctrl+Shift+L";
const QString Global::cutWindowKeySequence = "Ctrl+Shift+X";

const QByteArray Global::aesKey = "Q5tO3jUB0oe7FmWq";
const QByteArray Global::aesIv  = "RNNSR8XNbMTuOSGd";

QList<NoteModel *> Global::noteModelList = {};
QList<CategoryModel *> Global::categoryModelList = {};
QList<TagModel *> Global::tagsModelList = {};

ConfigModel *Global::configModel = new ConfigModel();
NoteModel *Global::openNoteModel = new NoteModel();
GitManager *Global::gitManager = new GitManager();
ThemeManager *Global::themeManager = new ThemeManager();

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

void Global::initCategoryModelList()
{
    categoryModelList.clear();

    QMap<QString, CategoryModel *> map;
    for (auto &&noteModel : noteModelList) {
//        if (map.contains(noteModel->categoryModel->getName())) {
//            auto *categoryModel = map[noteModel->categoryModel->getName()];
//            categoryModel->setCount(categoryModel->getCount() + 1);
//        }
//        else {
//            noteModel->categoryModel->setCount(1);
//            map[noteModel->categoryModel->getName()] = noteModel->categoryModel;
//        }
    }

    QStringList stringList = Tools::readerFileToList(repoCategoriesListPath);
    for (auto &&str : stringList) {
        if (!str.isEmpty() && !map.contains(str)) {
            map[str] = new CategoryModel(str);
        }
    }

    categoryModelList = map.values();

    if (stringList.length() == 0) {
        saveCategoryModelList();
    }
}

bool Global::addCategoryModelList(QList<CategoryModel *> list)
{
    bool isWriterFile = false;
    for (auto &&item : list) {
        bool hasName = false;
        for (auto &&categoryModel : categoryModelList) {
            if (categoryModel->getName() == item->getName()) {
                hasName = true;
            }
        }
        if (!hasName) {
            isWriterFile = true;
            categoryModelList.append(item);
        }
    }

    if (isWriterFile || list.length() == 0) {
        saveCategoryModelList();
    }

    return isWriterFile;
}

bool Global::hasInCategoryModelList(const QString &name)
{
    for (auto &&item : categoryModelList) {
        if (item->getName() == name) {
            return true;
        }
    }

    return false;
}

void Global::initTagModelList()
{
    tagsModelList.clear();

    QMap<QString, TagModel *> map;
    for (auto &&noteModel : noteModelList) {
//        for (auto &&tagsModel : *noteModel->tagsModelList) {
//            if (tagsModel->getName().isEmpty()) {
//                continue;
//            }
//            if (map.contains(tagsModel->getName())) {
//                auto *model = map[tagsModel->getName()];
//                model->setCount(model->getCount() + 1);
//            }
//            else {
//                tagsModel->setCount(1);
//                map[tagsModel->getName()] = tagsModel;
//            }
//        }
    }

    QStringList stringList = Tools::readerFileToList(repoTagsListPath);
    for (auto &&str : stringList) {
        if (!str.isEmpty() && !map.contains(str)) {
            map[str] = new TagModel(str);
        }
    }

    tagsModelList = map.values();

    if (stringList.length() == 0) {
        saveTagModelList();
    }
}

bool Global::addTagModelList(QList<TagModel *> list)
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
        saveTagModelList();
    }

    return isWriterFile;
}

NoteModel *Global::getNoteModelByUuid(const QString &uuid)
{
    for (auto &&noteModel :noteModelList) {
//        if (noteModel->contentModel->getUuid() == uuid) {
//            return noteModel;
//        }
    }

    return nullptr;
}

void Global::initConfigModel()
{
    Tools::createMkDir(appDataPath);
    QString jsonString = Tools::readerFile(appConfigPath);
    if (jsonString.isEmpty()) {
        configModel->serialize(appConfigPath);
    }
    else {
        configModel->unserialize(jsonString);
    }
}

void Global::setOpenNoteModel(NoteModel *noteModel)
{
    openNoteModel = noteModel;
//    configModel->setOpenNotesUuid(noteModel->contentModel->getUuid());

    addNoteModelToList(noteModel);
}

void Global::addNoteModelToList(NoteModel *noteModel)
{
    if (noteModelList.indexOf(noteModel) == -1) {
        noteModelList.append(noteModel);
        initCategoryModelList();
        initTagModelList();
    }
}

bool Global::hasInTagModelList(const QString &name)
{
    for (auto &&item : tagsModelList) {
        if (item->getName() == name) {
            return true;
        }
    }

    return false;
}

void Global::saveCategoryModelList()
{
    QString str = "";
    for (auto &&item : categoryModelList) {
        str += item->getName() + "\n";
    }
    Tools::writerFile(repoCategoriesListPath, str.trimmed());
}

void Global::saveTagModelList()
{
    QString str = "";
    for (auto &&item : tagsModelList) {
        str += item->getName() + "\n";
    }
    Tools::writerFile(repoTagsListPath, str.trimmed());
}

void Global::renameTagModelListInName(const QString oldName, const QString newName)
{
    for (auto &&item : noteModelList) {
//        for (auto &&tagsModel : *item->tagsModelList) {
//            if (tagsModel->getName() == oldName) {
//                tagsModel->setName(newName);
//                item->writerLocal();
//                break;
//            }
//        }
    }

    saveTagModelList();
}

void Global::renameCategoryModelListInName(const QString oldName, const QString newName)
{
    for (auto &&item : noteModelList) {
//        if (item->categoryModel->getName() == oldName) {
//            item->categoryModel->setName(newName);
//            item->writerLocal();
//        }
    }

    saveCategoryModelList();
}

void Global::initRepoLocalDir()
{
    Tools::createMkDir(repoNotePath);
}

int Global::initGitManager()
{
    if (QDir(Global::repoPath).exists()) {
        return gitManager->open(Global::repoPath.toUtf8().constData());
    }

    if (!configModel->getRepoUsername().isEmpty()
        && !configModel->getRepoPassword().isEmpty()
        && !configModel->getRepoUrl().isEmpty()) {
        gitManager->setUserPass(configModel->getRepoUsername().toUtf8().constData(),
                                configModel->getRepoPassword().toUtf8().constData());
        return gitManager->clone(configModel->getRepoUrl().toUtf8().constData(), repoPath.toUtf8().constData());
    }

    return gitManager->initLocalRepo(Global::repoPath.toUtf8().constData(), true);
}

