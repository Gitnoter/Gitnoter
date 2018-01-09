#include "singletimeout.h"
#include "tools.h"

SingleTimeout::SingleTimeout(Gitnoter::SingleTimerType type, QObject *parent) :
        QTimer(parent),
        mSingleTimerType(type)
{

}

void SingleTimeout::singleShot(int msec, QObject *receiver, const QString &member)
{
    if (0 == msec) {
        disconnect(this);
        if (isActive()) {
            stop();
        }
        return;
    }

    if (!isActive()) {
        connect(this, &QTimer::timeout, [=]() {
            QMetaObject::invokeMethod(receiver, Tools::qstringToConstData(member.mid(1, member.length() - 3)));
            if (Gitnoter::SingleTimeNone != mSingleTimerType) {
                stop();
                disconnect(this);
            }
        });
        start(msec);
        return;
    }

    if (Gitnoter::NoResetTimeout != mSingleTimerType) {
        stop();
        start(msec);
    }
}
