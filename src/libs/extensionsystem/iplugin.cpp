#include "iplugin.h"
#include "iplugin_p.h"
#include "pluginmanager.h"
#include "pluginspec.h"

/*!
    \class ExtensionSystem::IPlugin
    \inheaderfile extensionsystem/iplugin.h
    \inmodule QtCreator
    \ingroup mainclasses

    \brief IPlugin 类是一个抽象基类，每个插件必须实现一次。

    除了实际的插件库之外，插件还需要提供元数据，以便插件管理器能够找到插件、解析其依赖关系并加载它。更多信息，请参见 \l{Plugin Meta Data}。

    插件必须提供一个 IPlugin 类的实现，位于与其元数据中给出的name 属性匹配的库中。IPlugin 实现必须被导出并使用Q_PLUGIN_METADATA 宏
    使 Qt 的插件系统知晓，IID 设置为"org.qt-project.Qt.QtCreatorPlugin"。

    更多信息，请参见 \l{Plugin Life Cycle}。
*/

/*!
    \enum IPlugin::ShutdownFlag

    此枚举类型表示插件是同步关闭还是异步关闭。

    \value SynchronousShutdown
           插件同步关闭。
    \value AsynchronousShutdown
           插件在关闭之前需要执行异步操作。
*/

/*!
    \fn bool ExtensionSystem::IPlugin::initialize(const QStringList &arguments, QString *errorString)
    在插件加载完成并创建 IPlugin 实例后调用。

    依赖于此插件的其他插件的 initialize 函数将在此插件的 initialize 函数使用arguments 调用后才被调用。插件应在此函数中初始化其内部状态。

    返回初始化是否成功。如果不成功，应将errorString 设置为描述原因的用户可读消息。

    \sa extensionsInitialized()
    \sa delayedInitialize()
*/

/*!
    \fn void ExtensionSystem::IPlugin::extensionsInitialized()
    在调用 initialize() 函数之后，以及依赖于此插件的其他插件的 initialize()和extensionsInitialized() 函数被调用之后调用。

    在此函数中，插件可以假设依赖于此插件的插件已完全 \e {启动并运行}。这是在全局对象池中查找弱依赖插件提供的对象的好地方。

    \sa initialize()
    \sa delayedInitialize()
*/

/*!
    \fn bool ExtensionSystem::IPlugin::delayedInitialize()
    在所有插件的 extensionsInitialized() 函数被调用后，以及依赖于此插件的插件的delayedInitialize() 函数被调用后调用。

    插件的delayedInitialize() 函数在应用程序已经运行后被调用，与应用程序启动相比有几毫秒的延迟，各个delayedInitialize()
    函数调用之间也有延迟。为避免不必要的延迟，如果插件实际实现了此函数，应该从函数返回true，以指示下一个插件的delayedInitialize()
    调用应该延迟几毫秒，以便处理输入和绘制事件。

    如果插件需要进行非平凡的设置，这些设置不一定需要在启动时直接完成，但仍应在短时间内完成，则可以使用此函数。这可以大大减少感知到的插件
    或应用程序启动时间，而且只需很少的努力。

    \sa initialize()
    \sa extensionsInitialized()
*/

/*!
    \fn ExtensionSystem::IPlugin::ShutdownFlag ExtensionSystem::IPlugin::aboutToShutdown()
    在关闭序列中以与初始化相同的顺序调用，在插件以相反顺序被删除之前。

    此函数应用于断开与其他插件的连接、隐藏所有 UI，并总体上优化关闭过程。
    如果插件需要延迟实际关闭一段时间，例如需要等待外部进程完成以实现
    清洁关闭，插件可以从此函数返回 IPlugin::AsynchronousShutdown。
    这将使主事件循环在 aboutToShutdown() 序列完成后继续运行，直到所有
    请求异步关闭的插件都发送了 asynchronousShutdownFinished() 信号。

    此函数的默认实现不执行任何操作，并返回 IPlugin::SynchronousShutdown。

    如果插件需要在关闭前执行异步操作，则返回 IPlugin::AsynchronousShutdown。

    \sa asynchronousShutdownFinished()
*/

/*!
    \fn QObject *ExtensionSystem::IPlugin::remoteCommand(const QStringList &options,
                                           const QString &workingDirectory,
                                           const QStringList &arguments)

    当另一个 \QC 实例正在运行时使用-client 参数执行 \QC 时，
    将在运行的实例中调用插件的此函数。

   workingDirectory 参数指定调用进程的工作目录。例如，如果您在一个
    目录中，并执行{ qtcreator -client file.cpp}，调用进程的工作目录
    将传递给正在运行的实例，并且{file.cpp} 将被转换为从该目录开始的
    绝对路径。

    插件特定的参数在options 中传递，而其余参数在arguments 中传递。

    如果使用-block，则返回一个 QObject，该对象会阻塞命令直到它被销毁。
*/

/*!
    \fn void ExtensionSystem::IPlugin::asynchronousShutdownFinished()
    在异步关闭准备好继续关闭序列后由插件实现发送。

    \sa aboutToShutdown()
*/


using namespace ExtensionSystem;

/*!
    \internal
*/
IPlugin::IPlugin()
    : d(new Internal::IPluginPrivate())
{
}

/*!
    \internal
*/
IPlugin::~IPlugin()
{
    delete d;
    d = nullptr;
}

/*!
    Returns objects that are meant to be passed on to \l QTest::qExec().
    返回旨在传递给 QTest::qExec() 的对象。

    This function will be called if the user starts \QC with{-test PluginName} or{-test all}.
    如果用户使用 {-test PluginName} 或 {-test all} 启动，将调用此函数。

    The ownership of returned objects is transferred to caller.
    返回对象的所有权转移给调用者。
*/
QVector<QObject *> IPlugin::createTestObjects() const
{
    return {};
}

/*!
    返回与此插件对应的 PluginSpec。
    这在构造函数中不可用。
*/
PluginSpec *IPlugin::pluginSpec() const
{
    return d->pluginSpec;
}
