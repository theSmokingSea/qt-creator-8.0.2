#pragma once

#include "ilocatorfilter.h"

#include <utils/commandline.h>

#include <QQueue>
#include <QStringList>
#include <QTextCodec>

namespace Utils { class QtcProcess; }

namespace Core {
namespace Internal {

class ExecuteFilter : public Core::ILocatorFilter
{
    Q_OBJECT

    struct ExecuteData
    {
        Utils::CommandLine command;
        Utils::FilePath workingDirectory;
    };

public:
    ExecuteFilter();
    ~ExecuteFilter() override;
    QList<LocatorFilterEntry> matchesFor(QFutureInterface<LocatorFilterEntry> &future,
                                         const QString &entry) override;
    void accept(const LocatorFilterEntry &selection,
                QString *newText, int *selectionStart, int *selectionLength) const override;

private:
    void done();
    void readStandardOutput();
    void readStandardError();
    void runHeadCommand();

    void createProcess();
    void removeProcess();

    void saveState(QJsonObject &object) const final;
    void restoreState(const QJsonObject &object) final;

    QString headCommand() const;

    QQueue<ExecuteData> m_taskQueue;
    QStringList m_commandHistory;
    Utils::QtcProcess *m_process = nullptr;
    QTextCodec::ConverterState m_stdoutState;
    QTextCodec::ConverterState m_stderrState;
};

} // namespace Internal
} // namespace Core
