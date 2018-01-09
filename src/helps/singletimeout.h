#ifndef SINGLETIMEOUT_H
#define SINGLETIMEOUT_H

#include <QTimer>
#include <gitnoter.h>

class SingleTimeout : public QTimer
{
Q_OBJECT
public:
    explicit SingleTimeout(Gitnoter::SingleTimerType type = Gitnoter::ResetTimeout, QObject *parent = nullptr);

    void singleShot(int msec, QObject *receiver, const QString &member);

signals:

public slots:

public:
    Gitnoter::SingleTimerType resetTimeout() { return mSingleTimerType; }
    void setResetTimeout(Gitnoter::SingleTimerType type) { mSingleTimerType = type; }

private:
    Gitnoter::SingleTimerType mSingleTimerType;

};

#endif // SINGLETIMEOUT_H
