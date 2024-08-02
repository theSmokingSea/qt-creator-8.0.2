#pragma once

#include "utils_global.h"

#include <QMap>
#include <QMutex>
#include <QProcess>
#include <QThread>
#include <QWaitCondition>

#include <memory>

namespace Utils {

class CommandLine;
class ProcessResultData;
class QtcProcess;

class DeviceShellImpl;

class QTCREATOR_UTILS_EXPORT DeviceShell : public QObject
{
    Q_OBJECT

public:
    enum class State { FailedToStart = -1, Unknown = 0, Succeeded = 1 };

    struct RunResult
    {
        int exitCode = 0;
        QByteArray stdOut;
        QByteArray stdErr;
    };

    enum class ParseType {
        StdOut,
        StdErr,
        ExitCode,
    };

    DeviceShell();
    virtual ~DeviceShell();

    bool start();

    bool runInShell(const CommandLine &cmd, const QByteArray &stdInData = {});
    RunResult outputForRunInShell(const CommandLine &cmd, const QByteArray &stdInData = {});

    State state() const;

signals:
    void done(const ProcessResultData &resultData);

protected:
    virtual void startupFailed(const CommandLine &cmdLine);
    RunResult run(const CommandLine &cmd, const QByteArray &stdInData = {});

    void close();

private:
    virtual void setupShellProcess(QtcProcess *shellProcess);

    bool installShellScript();
    void closeShellProcess();

    void onReadyRead();

private:
    struct CommandRun : public RunResult
    {
        QWaitCondition *waiter;
    };

    std::unique_ptr<QtcProcess> m_shellProcess;
    QThread m_thread;
    int m_currentId{0};

    QMutex m_commandMutex;
    // QMap is used here to preserve iterators
    QMap<quint64, CommandRun> m_commandOutput;
    QByteArray m_commandBuffer;

    State m_shellScriptState = State::Unknown;
};

} // namespace Utils
