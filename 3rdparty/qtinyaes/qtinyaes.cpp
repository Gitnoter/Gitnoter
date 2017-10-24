#include "qtinyaes.h"
extern "C" {
#include <qtinyaes/aes.h>
}
#undef CBC
#undef ECB
#include <QtMath>
#include <QDebug>
#include <QtEndian>

const qint32 QTinyAes::BLOCKSIZE = 16;
#if defined(AES256) && (AES256 == 1)
const quint32 QTinyAes::KEYSIZE = 32;
#elif defined(AES192) && (AES192 == 1)
const quint32 QTinyAes::KEYSIZE = 24;
#else
const quint32 QTinyAes::KEYSIZE = 16;
#endif

QTinyAes::QTinyAes(QObject *parent) ://TODO auto-generate key
	QObject(parent),
	_mode(CBC),
	_key(),
	_iv()
{}

QTinyAes::QTinyAes(QTinyAes::CipherMode mode, const QByteArray &key, const QByteArray &iv, QObject *parent) :
	QObject(parent),
	_mode(mode),
	_key(),
	_iv()
{
	setKey(key);
	setIv(iv);
}

QTinyAes::~QTinyAes()
{
	memset(_key.data(), 0, _key.size());
	memset(_iv.data(), 0, _iv.size());
}

QTinyAes::CipherMode QTinyAes::mode() const
{
	return _mode;
}

QByteArray QTinyAes::key() const
{
	return _key;
}

QByteArray QTinyAes::iv() const
{
	return _iv;
}

void QTinyAes::setMode(QTinyAes::CipherMode mode)
{
	_mode = mode;
}

void QTinyAes::setKey(const QByteArray &key)
{
	Q_ASSERT_X(key.size() == QTinyAes::KEYSIZE, Q_FUNC_INFO, "The Key-Length is not a valid length! (Check QTinyAes::KEYSIZE)");
	_key = key;
}

void QTinyAes::resetKey()
{
	memset(_key.data(), 0, _key.size());
	_key.clear();
}

void QTinyAes::setIv(const QByteArray &iv)
{
	if(iv.isEmpty())
		resetIv();
	else {
		Q_ASSERT_X(iv.size() >= QTinyAes::BLOCKSIZE, Q_FUNC_INFO, "The initialisation vector must be at least QTinyAes::BLOCKSIZE bytes long (or empty)");
		if(iv.size() > QTinyAes::BLOCKSIZE)
			qWarning() << "IV is longer then QTinyAes::BLOCKSIZE - the rest will be truncated";
		_iv = iv.mid(0, QTinyAes::BLOCKSIZE);
	}
}

void QTinyAes::resetIv()
{
	memset(_iv.data(), 0, _iv.size());
	_iv.clear();
}

QByteArray QTinyAes::encrypt(QByteArray plain) const
{
	Q_ASSERT_X(!_key.isEmpty(), Q_FUNC_INFO, "The key must not be empty to encrypt data");
	preparePlainText(plain);
	QByteArray output(plain.size(), Qt::Uninitialized);

	switch(_mode) {
	case CBC:
		AES_CBC_encrypt_buffer((uint8_t*)output.data(),
							   (uint8_t*)plain.data(),
							   (uint32_t)plain.size(),
							   (uint8_t*)_key.data(),
							   (uint8_t*)(_iv.isEmpty() ? NULL : _iv.data()));
		break;
	case ECB:
		AES_ECB_encrypt((uint8_t*)plain.data(),
						(uint8_t*)_key.data(),
						(uint8_t*)output.data(),
						(uint32_t)plain.size());
		break;
	default:
		Q_UNREACHABLE();
		return QByteArray();
	}

	return output;

}

QByteArray QTinyAes::decrypt(QByteArray cipher) const
{
	Q_ASSERT_X(!_key.isEmpty(), Q_FUNC_INFO, "The key must not be empty to decrypt data");
	QByteArray output(cipher.size(), Qt::Uninitialized);

	switch(_mode) {
	case CBC:
		AES_CBC_decrypt_buffer((uint8_t*)output.data(),
							   (uint8_t*)cipher.data(),
							   (uint32_t)cipher.size(),
							   (uint8_t*)_key.data(),
							   (uint8_t*)(_iv.isEmpty() ? NULL : _iv.data()));
		break;
	case ECB:
		AES_ECB_decrypt((uint8_t*)cipher.data(),
						(uint8_t*)_key.data(),
						(uint8_t*)output.data(),
						(uint32_t)cipher.size());
		break;
	default:
		Q_UNREACHABLE();
		return QByteArray();
	}

	restorePlainText(output);
	return output;
}

QByteArray QTinyAes::cbcEncrypt(const QByteArray &key, const QByteArray &iv, const QByteArray &plain)
{
	return QTinyAes(QTinyAes::CBC, key, iv).encrypt(plain);
}

QByteArray QTinyAes::cbcDecrypt(const QByteArray &key, const QByteArray &iv, const QByteArray &cipher)
{
	return QTinyAes(QTinyAes::CBC, key, iv).decrypt(cipher);
}

QByteArray QTinyAes::ecbEncrypt(const QByteArray &key, const QByteArray &plain)
{
	return QTinyAes(QTinyAes::ECB, key).encrypt(plain);
}

QByteArray QTinyAes::ecbDecrypt(const QByteArray &key, const QByteArray &cipher)
{
	return QTinyAes(QTinyAes::ECB, key).decrypt(cipher);
}

void QTinyAes::preparePlainText(QByteArray &data)
{
	QByteArray dataSize(sizeof(quint32), (char)0);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
	qToBigEndian((quint32)data.size(), dataSize.data());
#else
	qToBigEndian((quint32)data.size(), (uchar*)dataSize.data());
#endif
	data.prepend(dataSize);
	data.append(QByteArray(QTinyAes::BLOCKSIZE - (data.size() % QTinyAes::BLOCKSIZE), 0));
}

void QTinyAes::restorePlainText(QByteArray &data)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
	auto dataLen = qFromBigEndian<quint32>(data.constData());
#else
	auto dataLen = qFromBigEndian<quint32>((const uchar*)data.constData());
#endif
	data = data.mid(sizeof(quint32), dataLen);
}
