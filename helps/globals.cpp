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
const QString Global::repoNoteTextPath = QDir(repoPath).filePath("notes/");
const QString Global::noteTextFileName = "markdown.md";
const QString Global::noteDataFileName = "data.json";
const QString Global::repoNoteDataPath = QDir(repoPath).filePath("data");
const QString Global::repoCategoriesListPath = QDir(repoNoteDataPath).filePath("categories.list");
const QString Global::repoTagsListPath = QDir(repoNoteDataPath).filePath("tags.list");

const QString Global::newNoteKeySequence = "Ctrl+N";
const QString Global::lockWindowKeySequence = "Ctrl+Shift+L";
const QString Global::cutWindowKeySequence = "Ctrl+Shift+X";

const QByteArray Global::aesKey = "Q5tO3jUB0oe7FmWq";
const QByteArray Global::aesIv  = "RNNSR8XNbMTuOSGd";

QList<NoteModel *> Global::noteModelList = {};
QList<CategoryModel *> Global::categoryModelList = {};
QList<TagModel *> Global::tagModelList = {};

ConfigModel *Global::configModel = new ConfigModel();
NoteModel *Global::openNoteModel = new NoteModel();
GitManager *Global::gitManager = new GitManager();
ThemeManager *Global::themeManager = new ThemeManager();

void Global::initNoteModelList()
{
    noteModelList.clear();
    QDir dir(repoNoteTextPath);

    for (auto &&mfi : dir.entryInfoList()) {
        if (mfi.fileName() == "." || mfi.fileName() == "..") {
            continue;
        }
        if (mfi.isDir()) {
            QDir dir2(mfi.absoluteFilePath());
            NoteModel *noteModel = new NoteModel(dir2.filePath(noteTextFileName), dir2.filePath(noteDataFileName));
            noteModelList.append(noteModel);
        }
    }
}

void Global::initCategoryModelList()
{
    categoryModelList.clear();
    QMap<QString, CategoryModel *> map;
    for (auto &&noteModel : noteModelList) {
        if (map.contains(noteModel->getCategory())) {
            CategoryModel *categoryModel = map[noteModel->getCategory()];
            categoryModel->setCount(categoryModel->getCount() + 1);
        }
        else {
            map[noteModel->getCategory()] = new CategoryModel(noteModel->getCategory(), 1);
        }
    }

    QStringList stringList = Tools::readerFileToList(repoCategoriesListPath);
    for (auto &&str : stringList) {
        if (!str.isEmpty() && !map.contains(str)) {
            map[str] = new CategoryModel(str);
        }
    }

    categoryModelList = map.values();

    saveCategoryModelList();
}

void Global::addCategoryModelToList(const QString categoryName)
{
    for (auto &&item : categoryModelList) {
        if (item->getName() == categoryName) {
            item->setCount(item->getCount() + 1);
            saveCategoryModelList();
            return;
        }
    }

    categoryModelList.append(new CategoryModel(categoryName, 1));
    saveCategoryModelList();
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
    tagModelList.clear();

    QMap<QString, TagModel *> map;
    for (auto &&noteModel : noteModelList) {
        for (auto &&item : noteModel->getTagList()) {
            if (map.contains(item)) {
                TagModel *tagModel = map[item];
                tagModel->setCount(tagModel->getCount() + 1);
            }
            else {
                map[item] = new TagModel(item, 1);
            }
        }
    }

    QStringList stringList = Tools::readerFileToList(repoTagsListPath);
    for (auto &&str : stringList) {
        if (!str.isEmpty() && !map.contains(str)) {
            map[str] = new TagModel(str);
        }
    }

    tagModelList = map.values();

    saveTagModelList();
}

void Global::addTagModelList(const QString tagName)
{
    if (tagName.isEmpty()) {
        return;
    }

    for (auto &&tagsModel : tagModelList) {
        if (tagsModel->getName() == tagName) {
            tagsModel->setCount(tagsModel->getCount() + 1);
            saveTagModelList();
            return;
        }
    }

    tagModelList.append(new TagModel(tagName, 1));
    saveTagModelList();
}

NoteModel *Global::getNoteModelByUuid(const QString &uuid)
{
    for (auto &&noteModel :noteModelList) {
        if (noteModel->getUuid() == uuid) {
            return noteModel;
        }
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
    configModel->setOpenNotesUuid(noteModel->getUuid());

    addNoteModelToList(noteModel);
}

void Global::addNoteModelToList(NoteModel *noteModel)
{
    if (noteModelList.indexOf(noteModel) == -1) {
        noteModelList.append(noteModel);
        addCategoryModelToList(Global::configModel->getCategoryName());
    }
}

bool Global::hasInTagModelList(const QString &name)
{
    for (auto &&item : tagModelList) {
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
    for (auto &&item : tagModelList) {
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
    Tools::createMkDir(repoNoteTextPath);
    Tools::createMkDir(repoNoteDataPath);
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

