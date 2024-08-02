#pragma once

#include "core_global.h"

#include <QString>

#define QTC_DECLARE_MYTESTDATADIR(PATH)                                          \
    class MyTestDataDir : public Core::Tests::TestDataDir                        \
    {                                                                            \
    public:                                                                      \
        MyTestDataDir(const QString &testDataDirectory = QString())              \
            : TestDataDir(QLatin1String(SRCDIR "/" PATH) + testDataDirectory) {} \
    };

namespace Core {
namespace Tests {

class CORE_EXPORT TestDataDir
{
public:
    TestDataDir(const QString &directory);

    QString file(const QString &fileName) const;
    QString directory(const QString &subdir = QString(), bool clean = true) const;

    QString path() const;

private:
    QString m_directory;
};

} // namespace Tests
} // namespace Core
