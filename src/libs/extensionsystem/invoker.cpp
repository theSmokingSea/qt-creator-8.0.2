#include "invoker.h"

namespace ExtensionSystem {

/*!
    \class ExtensionSystem::InvokerBase
    \internal
*/

/*!
    \class ExtensionSystem::Invoker
    \internal
*/
/*!
    \fn template <class Result> Result ExtensionSystem::invoke(QObject *target, const char *slot)
    通过 Qt 的元方法系统，按名称调用target 上的slot。

    返回元调用的结果。
*/

/*!
    \fn template <class Result, class T0> Result ExtensionSystem::invoke(QObject *target, const char *slot, const T0 &t0)
    通过 Qt 的元方法系统，按名称调用target 上的slot，并传入参数t0。

    返回元调用的结果。
*/

/*!
    \fn template <class Result, class T0, class T1> Result ExtensionSystem::invoke(QObject *target, const char *slot, const T0 &t0, const T1 &t1)
    通过 Qt 的元方法系统，按名称调用target 上的slot，并传入参数t0 和t1。

    返回元调用的结果。
*/

/*!
    \fn template <class Result, class T0, class T1, class T2> Result ExtensionSystem::invoke(QObject *target, const char *slot, const T0 &t0, const T1 &t1, const T2 &t2)
    通过 Qt 的元方法系统，按名称调用target 上的slot，并传入参数t0、\a t1 和t2。

    返回元调用的结果。
*/

InvokerBase::InvokerBase()
{
    lastArg = 0;
    useRet = false;
    nag = true;
    success = true;
    connectionType = Qt::AutoConnection;
    target = nullptr;
}

InvokerBase::~InvokerBase()
{
    if (!success && nag)
        qWarning("Could not invoke function '%s' in object of type '%s'.",
            sig.constData(), target->metaObject()->className());
}

bool InvokerBase::wasSuccessful() const
{
    nag = false;
    return success;
}

void InvokerBase::setConnectionType(Qt::ConnectionType c)
{
    connectionType = c;
}

void InvokerBase::invoke(QObject *t, const char *slot)
{
    target = t;
    success = false;
    sig.append(slot, qstrlen(slot));
    sig.append('(');
    for (int paramCount = 0; paramCount < lastArg; ++paramCount) {
        if (paramCount)
            sig.append(',');
        const char *type = arg[paramCount].name();
        sig.append(type, int(strlen(type)));
    }
    sig.append(')');
    sig.append('\0');
    int idx = target->metaObject()->indexOfMethod(sig.constData());
    if (idx < 0)
        return;
    QMetaMethod method = target->metaObject()->method(idx);
    if (useRet)
        success = method.invoke(target, connectionType, ret,
           arg[0], arg[1], arg[2], arg[3], arg[4],
           arg[5], arg[6], arg[7], arg[8], arg[9]);
    else
        success = method.invoke(target, connectionType,
           arg[0], arg[1], arg[2], arg[3], arg[4],
           arg[5], arg[6], arg[7], arg[8], arg[9]);
}

} // namespace ExtensionSystem
