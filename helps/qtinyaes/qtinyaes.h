#ifndef QTINYAES
#define QTINYAES

#include <QObject>
#include <QByteArray>
#include <QVector>

class QTinyAes : public QObject
{
	Q_OBJECT

	Q_PROPERTY(CipherMode mode READ mode WRITE setMode)
	Q_PROPERTY(QByteArray key READ key WRITE setKey RESET resetKey)
	Q_PROPERTY(QByteArray iv READ iv WRITE setIv RESET resetIv)

public:
	enum CipherMode {
		CBC,
		ECB
	};
	Q_ENUM(CipherMode)

	static const qint32 BLOCKSIZE;
	static const quint32 KEYSIZE;

	QTinyAes(QObject *parent = Q_NULLPTR);
	QTinyAes(CipherMode mode, const QByteArray &key, const QByteArray &iv = QByteArray(), QObject *parent = Q_NULLPTR);
	~QTinyAes();

	CipherMode mode() const;
	QByteArray key() const;
	QByteArray iv() const;

	Q_INVOKABLE QByteArray encrypt(QByteArray plain) const;
	Q_INVOKABLE QByteArray decrypt(QByteArray cipher) const;

	static QByteArray cbcEncrypt(const QByteArray &key, const QByteArray &iv, const QByteArray &plain);
	static QByteArray cbcDecrypt(const QByteArray &key, const QByteArray &iv, const QByteArray &cipher);
	static QByteArray ecbEncrypt(const QByteArray &key, const QByteArray &plain);
	static QByteArray ecbDecrypt(const QByteArray &key, const QByteArray &cipher);

public slots:
	void setMode(CipherMode mode);
	void setKey(const QByteArray &key);
	void resetKey();
	void setIv(const QByteArray &iv);
	void resetIv();

private:
	CipherMode _mode;
	QByteArray _key;
	QByteArray _iv;

	static void preparePlainText(QByteArray &data);
	static void restorePlainText(QByteArray &data);
};

#endif // QTINYAES

