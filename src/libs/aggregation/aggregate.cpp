#include "aggregate.h"

#include <QWriteLocker>
#include <QDebug>

/*!
    \namespace Aggregation
    \inmodule QtCreator
    \brief Aggregation命名空间包含了用于捆绑相关组件的支持，使得每个组件都能向外部暴露其他组件的属性和行为。
    被捆绑到一个聚合体中的组件可以相互进行\e转换，并且具有耦合的生命周期。详细信息和示例请参见Aggregation::Aggregate的文档。
*/

/*!
    \class Aggregation::Aggregate
    \inheaderfile aggregation/aggregate.h
    \inmodule QtCreator
    \ingroup mainclasses
    \threadsafe

    \brief Aggregate类定义了一个可以被视为一个单元的相关组件集合。
    聚合体是一个作为单元处理的组件集合， 使得集合中的每个组件都能向外部暴露聚合体中其他组件的属性和行为。
    具体来说，这意味着：
    \list
    \li 它们可以相互\e转换（使用query()和query_all()函数）。
    \li 它们的生命周期是耦合的。也就是说，只要其中一个被删除，所有的都会被删除。

    \endlist
    组件可以是任何派生自QObject的类型。

    你可以使用聚合体来通过聚合模拟多重继承。
    假设我们有以下代码：
    \code
        using namespace Aggregation;
        class MyInterface : public QObject { ........ };
        class MyInterfaceEx : public QObject { ........ };
        [...]
        MyInterface *object = new MyInterface; // this is single inheritance
    \endcode
    query函数的工作方式类似于普通对象的qobject_cast()：
    \code
        Q_ASSERT(query<MyInterface>(object) == object);
        Q_ASSERT(query<MyInterfaceEx>(object) == 0);
    \endcode
    如果我们希望\c object也实现\c MyInterfaceEx类，但不想或无法使用多重继承，我们可以在任何时候使用聚合体：
    \code
        MyInterfaceEx *objectEx = new MyInterfaceEx;
        Aggregate *aggregate = new Aggregate;
        aggregate->add(object);
        aggregate->add(objectEx);
    \endcode
    聚合体将这两个对象捆绑在一起。 如果我们有集合中的任何一部分，我们就能获得所有部分：
    \code
        Q_ASSERT(query<MyInterface>(object) == object);
        Q_ASSERT(query<MyInterfaceEx>(object) == objectEx);
        Q_ASSERT(query<MyInterface>(objectEx) == object);
        Q_ASSERT(query<MyInterfaceEx>(objectEx) == objectEx);
    \endcode
    以下操作会删除所有三个对象：\c object、\c objectEx和\c aggregate：
    \code
        delete objectEx;
        // or delete object;
        // or delete aggregate;
    \endcode

    支持聚合的代码永远不使用qobject_cast()。它总是使用
    Aggregation::query()，该函数在退化情况下的行为类似于qobject_cast()。
*/

/*!
    \fn template <typename T> T *Aggregation::Aggregate::component()

    返回给定类型的组件（如果存在）的模板函数。
    如果有多个该类型的组件，则返回其中一个随机选择的组件。

    \sa Aggregate::components(), add()
*/

/*!
    \fn template <typename T> QList<T *> Aggregation::Aggregate::components()

    返回所有给定类型的组件（如果存在）的模板函数。

    \sa Aggregate::component(), add()
*/

/*!
    \relates Aggregation::Aggregate
    \fn template <typename T> T *Aggregation::query<T *>(QObject *obj)

    执行一个动态转换，该转换意识到\a obj可能属于的聚合体。如果\a obj本身就是请求的类型，它会被简单地转换并返回。
    否则，如果\a obj属于一个聚合体，则会检查其所有组件。如果它不属于任何聚合体，则返回null。

    \sa Aggregate::component()
*/

/*!
    \relates Aggregation::Aggregate
    \fn template <typename T> QList<T *> Aggregation::query_all<T *>(QObject *obj)

    如果\a obj属于一个聚合体，则返回所有可以转换为给定类型的组件。
    否则，如果\a obj是请求的类型，则返回\a obj。

    \sa Aggregate::components()
*/

/*!
    \fn void Aggregation::Aggregate::changed()

    当一个组件被添加到聚合体或从聚合体中移除时，会发出此信号。

    \sa add(), remove()
*/

using namespace Aggregation;

// 如果\a obj有聚合体对象，则返回该聚合体对象。否则返回0。
Aggregate *Aggregate::parentAggregate(QObject *obj)
{
    QReadLocker locker(&lock());
    return aggregateMap().value(obj);
}

QHash<QObject *, Aggregate *> &Aggregate::aggregateMap()
{
    static QHash<QObject *, Aggregate *> map;
    return map;
}

QReadWriteLock &Aggregate::lock()
{
    static QReadWriteLock lock;
    return lock;
}


// 使用给定的\a parent创建一个新的聚合体。parent直接传递给类的QObject部分，除此之外不做其他用途。
Aggregate::Aggregate(QObject *parent)
    : QObject(parent)
{
    QWriteLocker locker(&lock());
    aggregateMap().insert(this, this);
}


// 删除聚合体会自动删除其所有组件。
Aggregate::~Aggregate()
{
    QList<QObject *> components;
    {
        QWriteLocker locker(&lock());
        for (QObject *component : qAsConst(m_components)) {
            disconnect(component, &QObject::destroyed, this, &Aggregate::deleteSelf);
            aggregateMap().remove(component);
        }
        components = m_components;
        m_components.clear();
        aggregateMap().remove(this);
    }
    qDeleteAll(components);
}

void Aggregate::deleteSelf(QObject *obj)
{
    {
        QWriteLocker locker(&lock());
        aggregateMap().remove(obj);
        m_components.removeAll(obj);
    }
    delete this;
}

/*!
    Adds the \a component to the aggregate.

    You cannot add a component that is part of a different aggregate or an aggregate itself.

    \sa remove()
*/
void Aggregate::add(QObject *component)
{
    if (!component)
        return;
    {
        QWriteLocker locker(&lock());
        Aggregate *parentAggregation = aggregateMap().value(component);
        if (parentAggregation == this)
            return;
        if (parentAggregation) {
            qWarning() << "Cannot add a component that belongs to a different aggregate" << component;
            return;
        }
        m_components.append(component);
        connect(component, &QObject::destroyed, this, &Aggregate::deleteSelf);
        aggregateMap().insert(component, this);
    }
    emit changed();
}

/*!
    Removes the \a component from the aggregate.

    \sa add()
*/
void Aggregate::remove(QObject *component)
{
    if (!component)
        return;
    {
        QWriteLocker locker(&lock());
        aggregateMap().remove(component);
        m_components.removeAll(component);
        disconnect(component, &QObject::destroyed, this, &Aggregate::deleteSelf);
    }
    emit changed();
}
