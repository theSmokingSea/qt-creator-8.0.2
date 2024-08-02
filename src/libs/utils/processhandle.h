#pragma once

#include "utils_global.h"

#include <QMetaType>

namespace Utils {

class QTCREATOR_UTILS_EXPORT ProcessHandle
{
public:
    ProcessHandle();
    explicit ProcessHandle(qint64 pid);

    bool isValid() const;
    void setPid(qint64 pid);
    qint64 pid() const;

    bool activate();

private:
    bool equals(const ProcessHandle &) const;

    friend bool operator==(const ProcessHandle &p, const ProcessHandle &q) { return p.equals(q); }
    friend bool operator!=(const ProcessHandle &p, const ProcessHandle &q) { return !p.equals(q); }

    qint64 m_pid;
};

} // Utils

Q_DECLARE_METATYPE(Utils::ProcessHandle)
