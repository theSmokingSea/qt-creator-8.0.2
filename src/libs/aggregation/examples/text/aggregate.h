#pragma once

#include <QObject>
#include <QList>
#include <QHash>
#include <QReadWriteLock>
#include <QReadLocker>

namespace Aggregation {

// 数据存储结构 如下 二层结构
// Aggregate
//      - QObject
//      - QObject
// Aggregate
//      - QObject
//      - QObject
//      - QObject
class Aggregate : public QObject
{
    Q_OBJECT

public:
    Aggregate(QObject *parent = nullptr);
    ~Aggregate() override;

    // 插件部分往这里面添加控件
    void add(QObject *component);
    // 目前仅findmacrohandler.cpp（macro）插件用到了 其他插件都没有使用
    // macro录制宏
    void remove(QObject *component);

    template <typename T> T *component() {
        QReadLocker locker(&lock());
        for (QObject *component : qAsConst(m_components)) {
            if (T *result = qobject_cast<T *>(component))
                return result;
        }
        return nullptr;
    }

    template <typename T> QList<T *> components() {
        QReadLocker locker(&lock());
        QList<T *> results;
        for (QObject *component : qAsConst(m_components)) {
            if (T *result = qobject_cast<T *>(component)) {
                results << result;
            }
        }
        return results;
    }

    static Aggregate *parentAggregate(QObject *obj);
    static QReadWriteLock &lock();

signals:
    void changed();

private:
    void deleteSelf(QObject *obj);

    static QHash<QObject *, Aggregate *> &aggregateMap();

    QList<QObject *> m_components;
};

// get a component via global template function
template <typename T> T *query(Aggregate *obj)
{
    if (!obj)
        return nullptr;
    return obj->template component<T>();
}

template <typename T> T *query(QObject *obj)
{
    if (!obj)
        return nullptr;
    T *result = qobject_cast<T *>(obj);
    if (!result) {
        QReadLocker locker(&Aggregate::lock());
        Aggregate *parentAggregation = Aggregate::parentAggregate(obj);
        result = (parentAggregation ? query<T>(parentAggregation) : nullptr);
    }
    return result;
}

// get all components of a specific type via template function
template <typename T> QList<T *> query_all(Aggregate *obj)
{
    if (!obj)
        return QList<T *>();
    return obj->template components<T>();
}

template <typename T> QList<T *> query_all(QObject *obj)
{
    if (!obj)
        return QList<T *>();
    QReadLocker locker(&Aggregate::lock());
    Aggregate *parentAggregation = Aggregate::parentAggregate(obj);
    QList<T *> results;
    if (parentAggregation)
        results = query_all<T>(parentAggregation);
    else if (T *result = qobject_cast<T *>(obj))
        results.append(result);
    return results;
}

} // namespace Aggregation
