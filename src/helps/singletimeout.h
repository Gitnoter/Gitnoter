#ifndef SINGLETIMEOUT_H
#define SINGLETIMEOUT_H

#include <QTimer>

class SingleTimeout : public QTimer
{
Q_OBJECT
public:
    explicit SingleTimeout(bool resetTimeout = true, QObject *parent = nullptr);

    void singleShot(int msec, QObject *receiver, const QString &member);

signals:

public slots:

public:
    bool resetTimeout() { return mResetTimeout; }
    void setResetTimeout(bool b) { mResetTimeout = b; }

private:
    bool mResetTimeout;

};

#endif // SINGLETIMEOUT_H
