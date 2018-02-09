#include "notemodel.h"
#include "tools.h"
#include "globals.h"
#include "notelistcellwidget.h"

#include <hoedown/html.h>

#include <QUrl>
#include <QMimeDatabase>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QTemporaryFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

NoteModel::NoteModel(const QString &noteText)
{
    clear();
    setNoteText(noteText);
}

NoteModel::NoteModel(const QString &textPath, const QString &dataPath)
{
    setNoteText(Tools::readerFileString(textPath));
    setNoteData(Tools::readerFileString(dataPath));
}

QString NoteModel::getNoteDir()
{
    if (mUuid.isEmpty()) {
        setUuid();
    }

    return QDir(__RepoNoteTextPath__).filePath(getUuid());
}


QString NoteModel::getMarkdownHtml()
{
    // get the decrypted note text (or the normal note text if there isn't any)
    hoedown_renderer *renderer = hoedown_html_renderer_new(HOEDOWN_HTML_USE_XHTML, 32);

    // we want to show quotes in the html, so we don't translate them into
    // `<q>` tags
    // HOEDOWN_EXT_MATH and HOEDOWN_EXT_MATH_EXPLICIT don't seem to do anything
    hoedown_extensions extensions =
            (hoedown_extensions) ((HOEDOWN_EXT_BLOCK | HOEDOWN_EXT_SPAN |
                                   HOEDOWN_EXT_MATH_EXPLICIT) & ~HOEDOWN_EXT_QUOTE);
    hoedown_document *document = hoedown_document_new(renderer, extensions, 32);

    QString noteText = getNoteText();
    QString windowsSlash = "";

#ifdef Q_OS_WIN32
    // we need an other slash for Windows
    windowsSlash = "/";
#endif

    // parse for relative file urls and make them absolute
    // (for example to show images under the note path)
    noteText.replace(QRegularExpression(
            "([\\(<])" + __FileScheme__ + "://" + "([^\\/].+?)([\\)>])"), "\\1file://" + windowsSlash + getNoteDir() + "/\\2\\3");

    unsigned char *sequence = (unsigned char *) qstrdup(noteText.toUtf8().constData());
    qint64 length = static_cast<qint64>(strlen((char *) sequence));

    // return an empty string if the note is empty
    if (length == 0) {
        return "";
    }

    hoedown_buffer *html = hoedown_buffer_new(static_cast<size_t>(length));

    // render markdown html
    hoedown_document_render(document, html, sequence, static_cast<size_t>(length));

    // get markdown html
    QString result = QString::fromUtf8((char *) html->data, static_cast<int>(html->size));

    /* Cleanup */
    free(sequence);
    hoedown_buffer_free(html);
    hoedown_document_free(document);
    hoedown_html_renderer_free(renderer);

    // set the stylesheet for the <code> blocks
    // do some more code formatting
    QString codeStyleSheet =
            "pre,code{padding: 16px;overflow: auto;line-height: 1.45em;background-color: #f1f1f1;border-radius: 3px;}";

    result.replace("<pre><code>", "<pre>").replace("</code></pre>", "</pre>");  // remove double code blocks
    result.replace(QRegularExpression("<del>([^<]+)<\\/del>"), "<s>\\1</s>");   // correct the strikeout tag

    result = QString("<html><head><meta charset=\"utf-8\"/><style>"
                             "h1 { margin: 5px 0 20px 0;}"
                             "h2, h3 { margin: 10px 0 15px 0;}"
                             "img { width: 100%; }"
                             "a { color: #FF9137; text-decoration: none;} %1"
                             "</style></head><body>%2</body></html>").arg(codeStyleSheet, result);

    return result;
}

QString NoteModel::htmlToMarkdown(QString text) {
    // replace Windows line breaks
    text.replace(QRegularExpression("\r\n"), "\n");

    // remove all null characters
    // we can get those from Google Chrome via the clipboard
    text.remove(QChar(0));

    // remove some blocks
    text.remove(QRegularExpression(
            "<head.*?>(.+?)<\\/head>",
            QRegularExpression::CaseInsensitiveOption |
            QRegularExpression::DotMatchesEverythingOption));

    text.remove(QRegularExpression(
            "<script.*?>(.+?)<\\/script>",
            QRegularExpression::CaseInsensitiveOption |
            QRegularExpression::DotMatchesEverythingOption));

    text.remove(QRegularExpression(
            "<style.*?>(.+?)<\\/style>",
            QRegularExpression::CaseInsensitiveOption |
            QRegularExpression::DotMatchesEverythingOption));

    // replace some html tags with markdown
    text.replace(QRegularExpression(
            "<strong.*?>(.+?)<\\/strong>",
            QRegularExpression::CaseInsensitiveOption |
            QRegularExpression::DotMatchesEverythingOption), "**\\1**");
    text.replace(QRegularExpression(
            "<b.*?>(.+?)<\\/b>",
            QRegularExpression::CaseInsensitiveOption |
            QRegularExpression::DotMatchesEverythingOption), "**\\1**");
    text.replace(QRegularExpression(
            "<em.*?>(.+?)<\\/em>",
            QRegularExpression::CaseInsensitiveOption |
            QRegularExpression::DotMatchesEverythingOption), "*\\1*");
    text.replace(QRegularExpression(
            "<i.*?>(.+?)<\\/i>",
            QRegularExpression::CaseInsensitiveOption |
            QRegularExpression::DotMatchesEverythingOption), "*\\1*");
    text.replace(QRegularExpression(
            "<pre.*?>(.+?)<\\/pre>",
            QRegularExpression::CaseInsensitiveOption |
            QRegularExpression::DotMatchesEverythingOption),
                 "\n```\n\\1\n```\n");
    text.replace(QRegularExpression(
            "<code.*?>(.+?)<\\/code>",
            QRegularExpression::CaseInsensitiveOption |
            QRegularExpression::DotMatchesEverythingOption),
                 "\n```\n\\1\n```\n");
    text.replace(QRegularExpression(
            "<h1.*?>(.+?)<\\/h1>",
            QRegularExpression::CaseInsensitiveOption |
            QRegularExpression::DotMatchesEverythingOption), "\n# \\1\n");
    text.replace(QRegularExpression(
            "<h2.*?>(.+?)<\\/h2>",
            QRegularExpression::CaseInsensitiveOption |
            QRegularExpression::DotMatchesEverythingOption), "\n## \\1\n");
    text.replace(QRegularExpression(
            "<h3.*?>(.+?)<\\/h3>",
            QRegularExpression::CaseInsensitiveOption |
            QRegularExpression::DotMatchesEverythingOption), "\n### \\1\n");
    text.replace(QRegularExpression(
            "<h4.*?>(.+?)<\\/h4>",
            QRegularExpression::CaseInsensitiveOption |
            QRegularExpression::DotMatchesEverythingOption), "\n#### \\1\n");
    text.replace(QRegularExpression(
            "<h5.*?>(.+?)<\\/h5>",
            QRegularExpression::CaseInsensitiveOption |
            QRegularExpression::DotMatchesEverythingOption), "\n##### \\1\n");
    text.replace(QRegularExpression(
            "<li.*?>(.+?)<\\/li>",
            QRegularExpression::CaseInsensitiveOption |
            QRegularExpression::DotMatchesEverythingOption), "- \\1");
    text.replace(QRegularExpression(
            "<br.*?>",
            QRegularExpression::CaseInsensitiveOption), "\n");
    text.replace(QRegularExpression(
            "<a[^>]+href=\"(.+?)\".*?>(.+?)<\\/a>",
            QRegularExpression::CaseInsensitiveOption |
            QRegularExpression::DotMatchesEverythingOption), "[\\2](\\1)");
    text.replace(QRegularExpression(
            "<p.*?>(.+?)</p>",
            QRegularExpression::CaseInsensitiveOption |
            QRegularExpression::DotMatchesEverythingOption), "\n\n\\1\n\n");

    // replace multiple line breaks
    text.replace(QRegularExpression("\n\n+"), "\n\n");

    return text;
}

QString NoteModel::downloadUrlToMedia(QString url, NoteModel *noteModel, bool returnUrlOnly) {
    QByteArray imageData = Tools::downloadUrlToData(QUrl(url));
    if (imageData.isEmpty()) { return ""; }

    // remove strings like "?b=16068071000" and non-characters from the suffix
    url.remove(QRegularExpression("\\?.+$")).remove(QRegularExpression("[^a-zA-Z0-9]"));

    const QString saveName = Tools::getShortUuid();
    const QStringList splitList = url.split("/", QString::SkipEmptyParts).last().split(".");
    const QString name = splitList.length() >= 2 ? splitList.first() : "";
    const QString suffix = splitList.length() >= 2 ? splitList.last() : "image";
    const QString savePath = noteModel->getNoteDir() + QDir::separator() + saveName + "." + suffix;

    Tools::writerFile(savePath, imageData);

    return returnUrlOnly
           ? savePath : "![" + name + "](" + __FileScheme__ + "://" + saveName + "." + suffix + ")";
}

QString NoteModel::byteArrayToMedia(const QByteArray &byteArray, NoteModel *noteModel, bool returnUrlOnly)
{

    return QString();
}

QString NoteModel::encodeCssFont(const QFont& refFont) {
//-----------------------------------------------------------------------
// This function assembles a CSS Font specification string from
// a QFont. This supports most of the QFont attributes settable in
// the Qt 4.8 and Qt 5.3 QFontDialog.
//
// (1) Font Family
// (2) Font Weight (just bold or not)
// (3) Font Style (possibly Italic or Oblique)
// (4) Font Size (in either pixels or points)
// (5) Decorations (possibly Underline or Strikeout)
//
// Not supported: Writing System (e.g. Latin).
//
// See the corresponding decode function, below.
// QFont decodeCssFontString (const QString cssFontStr)
//-----------------------------------------------------------------------

    QStringList fields; // CSS font attribute fields

// ***************************************************
// *** (1) Font Family: Primary plus Substitutes ***
// ***************************************************

    const QString family = refFont.family();

// NOTE [9-2014, Qt 4.8.6]: This isn't what I thought it was. It
// does not return a list of "fallback" font faces (e.g. Georgia,
// Serif for "Times New Roman"). In my testing, this is always
// returning an empty list.
//
    QStringList famSubs = QFont::substitutes (family);

    if (!famSubs.contains (family))
        famSubs.prepend (family);

    static const QChar DBL_QUOT ('"');
    const int famCnt = famSubs.count();
    QStringList famList;
    for (int inx = 0; inx < famCnt; ++inx)
    {
// Place double quotes around family names having space characters,
// but only if double quotes are not already there.
//
        const QString fam = famSubs [inx];
        if (fam.contains (' ') && !fam.startsWith (DBL_QUOT))
            famList << (DBL_QUOT + fam + DBL_QUOT);
        else
            famList << fam;
    }

    const QString famStr = QString ("font-family: ") + famList.join (", ");
    fields << famStr;

// **************************************
// *** (2) Font Weight: Bold or Not ***
// **************************************

    const bool bold = refFont.bold();
    if (bold)
        fields << "font-weight: bold";

// ****************************************************
// *** (3) Font Style: possibly Italic or Oblique ***
// ****************************************************

    const QFont::Style style = refFont.style();
    switch (style)
    {
        case QFont::StyleNormal: break;
        case QFont::StyleItalic: fields << "font-style: italic"; break;
        case QFont::StyleOblique: fields << "font-style: oblique"; break;
    }

// ************************************************
// *** (4) Font Size: either Pixels or Points ***
// ************************************************

    const double sizeInPoints = refFont.pointSizeF(); // <= 0 if not defined.
    const int sizeInPixels = refFont.pixelSize(); // <= 0 if not defined.
    if (sizeInPoints > 0.0)
        fields << QString ("font-size: %1pt") .arg (sizeInPoints);
    else if (sizeInPixels > 0)
        fields << QString ("font-size: %1px") .arg (sizeInPixels);

// ***********************************************
// *** (5) Decorations: Underline, Strikeout ***
// ***********************************************

    const bool underline = refFont.underline();
    const bool strikeOut = refFont.strikeOut();

    if (underline && strikeOut)
        fields << "text-decoration: underline line-through";
    else if (underline)
        fields << "text-decoration: underline";
    else if (strikeOut)
        fields << "text-decoration: line-through";

    const QString cssFontStr = fields.join ("; ");
    return cssFontStr;
}

QString NoteModel::getStringFormTagList() const
{
    QString tagsString = "";
    for (auto &&item : tagList) {
        tagsString += item + __TagSplit__;
    }
    tagsString.chop(QString(__TagSplit__).length());

    return tagsString;
}

void NoteModel::setNoteData(const QString &noteData)
{
    QJsonParseError e{};
    QJsonDocument jsonDoc = QJsonDocument::fromJson(noteData.toUtf8(), &e);
    if(e.error == QJsonParseError::NoError && !jsonDoc.isNull())
    {
        QJsonObject jsonObject = jsonDoc.object();
        mUuid = jsonObject.find("uuid").value().toString();
        mCreateDate = jsonObject.find("createDate").value().toInt();
        mUpdateDate = jsonObject.find("updateDate").value().toInt();
        category = jsonObject.find("category").value().toString();
        isDelete = jsonObject.find("isDelete").value().toInt();

        tagList.clear();
        QJsonArray tagListArray = jsonObject.find("tagList").value().toArray();
        for (auto &&tag : tagListArray) {
            tagList.append(tag.toString());
        }
    }
}

QString NoteModel::getNoteData() const
{
    QJsonObject jsonObject;
    jsonObject.insert("uuid", mUuid);
    jsonObject.insert("createDate", mCreateDate);
    jsonObject.insert("updateDate", mUpdateDate);
    jsonObject.insert("category", category);
    jsonObject.insert("isDelete", isDelete);

    QJsonArray tagListArray;
    for (int i = 0; i < tagList.length(); ++i) {
        tagListArray.push_back(tagList[i]);
    }
    jsonObject.insert("tagList", tagListArray);

    return QString(QJsonDocument(jsonObject).toJson());
}

void NoteModel::clear()
{
    setUuid();
    setNoteText();
    setCreateDate(0);
    setUpdateDate(0);
    setTagList();
    setCategory();
    setIsDelete();
}

void NoteModel::saveNoteToLocal()
{
    QString path = getNoteDir();
    Tools::createMkDir(path);
    Tools::writerFile(QDir(path).filePath(__NoteTextFileName__), getNoteText().toUtf8());
    Tools::writerFile(QDir(path).filePath(__NoteDataFileName__), getNoteData().toUtf8());
}

void NoteModel::saveNoteTextToLocal()
{
    QString path = getNoteDir();
    Tools::createMkDir(path);
    Tools::writerFile(QDir(path).filePath(__NoteTextFileName__), getNoteText().toUtf8());
}

void NoteModel::saveNoteDataToLocal()
{
    QString path = getNoteDir();
    Tools::createMkDir(path);
    Tools::writerFile(QDir(path).filePath(__NoteDataFileName__), getNoteData().toUtf8());
}

QString NoteModel::getUuid()
{
    return mUuid;
}

void NoteModel::setCreateDate(const QString createDate)
{
    mCreateDate = createDate.isEmpty() ? (int) QDateTime::currentSecsSinceEpoch()
                                       : Tools::timestampFromDateTime(createDate);
}

void NoteModel::setCreateDate(int createDate)
{
    mCreateDate = createDate == 0 ? (int) QDateTime::currentSecsSinceEpoch() : createDate;
}

void NoteModel::setUpdateDate(const QString updateDate)
{
    mUpdateDate = updateDate.isEmpty() ? (int) QDateTime::currentSecsSinceEpoch()
                                       : Tools::timestampFromDateTime(updateDate);
}

void NoteModel::setUpdateDate(int updateDate)
{
    mUpdateDate = updateDate == 0 ? (int) QDateTime::currentSecsSinceEpoch() : updateDate;
}

void NoteModel::setNoteText(QString body)
{
    mNoteText = body;

    QTextStream in(&body);
    while (!in.atEnd()) {
        QString lineText = in.readLine().trimmed();
        if (!lineText.isEmpty()) {
            mTitle = lineText.replace(QRegularExpression("^[#]*[ ]*"), "");
            break;
        }
    }
}

void NoteModel::setUuid(QString uuid)
{
    mUuid = uuid.isEmpty() ? Tools::getUuid() : uuid;
}

int NoteModel::getCreateDate()
{
    return mCreateDate;
}

int NoteModel::getUpdateDate()
{
    return mUpdateDate;
}

const QString NoteModel::getCreateDateString()
{
    return Tools::timestampToDateTime(mCreateDate);
}

const QString NoteModel::getUpdateDateString()
{
    return Tools::timestampToDateTime(mUpdateDate);
}

QString NoteModel::getNoteText()
{
    return mNoteText;
}

const QStringList &NoteModel::getTagList() const
{
    return tagList;
}

void NoteModel::setTagList(const QStringList &tagList)
{
    NoteModel::tagList = tagList;
}

const QString &NoteModel::getCategory() const
{
    return category;
}

void NoteModel::setCategory(const QString &category)
{
    NoteModel::category = category;
}

const QString &NoteModel::getTitle() const
{
    return mTitle;
}

void NoteModel::setTitle(const QString &title)
{
    NoteModel::mTitle = title;
}

int NoteModel::getIsDelete() const
{
    return isDelete;
}

void NoteModel::setIsDelete(int isDelete)
{
    NoteModel::isDelete = isDelete;
}

QString NoteModel::getShortUuid() const
{
    return mUuid.mid(0, 7);
}

QString NoteModel::getFilePath(QString noteFilePath) {
    return "file://" + QDir(getNoteDir()).filePath(noteFilePath.replace(__FileScheme__ + "://", ""));
}

const QString NoteModel::createMediaMarkdown(const QString &fileName, const QString &baseName, Gitnoter::MediaType type)
{
    if (Gitnoter::Image == type) {
        return "![" + baseName + "](" + __FileScheme__ + "://" + fileName + ")";
    }
    else if (Gitnoter::Accessory == type) {
        return "[" + baseName + "](" + __FileScheme__ + "://" + fileName + ")";
    }

    return "";
}

const QString NoteModel::saveMediaToLocal(QString filePath)
{
    const QFileInfo fileInfo = QFileInfo(filePath);
    Gitnoter::MediaType type = Gitnoter::Accessory;
    filePath = filePath.replace("file://", "");

    QFile::copy(filePath, QDir(getNoteDir()).filePath(fileInfo.fileName()));

    if (QStringList({"jpg", "jpeg", "png", "gif"}).contains(fileInfo.suffix(), Qt::CaseInsensitive)) {
        type = Gitnoter::Image;
    }

    return createMediaMarkdown(fileInfo.fileName(), fileInfo.baseName(), type);
}

const QString NoteModel::saveMediaToLocal(const QByteArray &data, const QString &suffix)
{
    const QString baseName = Tools::getShortUuid();
    const QString fileName = baseName + "." + suffix;
    Gitnoter::MediaType type = Gitnoter::Accessory;

    if (QStringList({"jpg", "jpeg", "png", "gif"}).contains(suffix, Qt::CaseInsensitive)) {
        type = Gitnoter::Image;
    }

    Tools::writerFile(QDir(getNoteDir()).filePath(fileName), data);

    return createMediaMarkdown(fileName, baseName, type);
}

const QString NoteModel::saveMediaToLocal(const QImage &data)
{
    const QString baseName = Tools::getShortUuid();
    const QString fileName = baseName + ".jpg";

    data.save(QDir(getNoteDir()).filePath(fileName), "JPG");

    return createMediaMarkdown(fileName, baseName);
}
