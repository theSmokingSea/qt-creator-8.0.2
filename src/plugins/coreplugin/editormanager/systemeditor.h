#pragma once

#include "iexternaleditor.h"

namespace Core {
namespace Internal {

class SystemEditor : public IExternalEditor
{
    Q_OBJECT

public:
    explicit SystemEditor();

    bool startEditor(const Utils::FilePath &filePath, QString *errorMessage) override;
};

} // namespace Internal
} // namespace Core
