#include "singletimeout.h"
#include "tools.h"

SingleTimeout::SingleTimeout(bool resetTimeout, QObject *parent) :
        QTimer(parent),
        mResetTimeout(resetTimeout)
{

}

void SingleTimeout::singleShot(int msec, QObject *receiver, const QString &member)
{
    if (!isActive()) {
        connect(this, &QTimer::timeout, [=]() {
            QMetaObject::invokeMethod(receiver, Tools::qstringToConstData(member.mid(1, member.length() - 3)));
            stop();
            disconnect(this);
        });
        start(msec);
        return;
    }

    if (mResetTimeout) {
        stop();
        start(msec);
    }
}
