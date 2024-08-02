#pragma once

#include "core_global.h"

#include <utils/filepath.h>

namespace Core {

class CORE_EXPORT PatchTool
{
public:
    static Utils::FilePath patchCommand();
    static void setPatchCommand(const Utils::FilePath &newCommand);

    // Utility to run the 'patch' command
    static bool runPatch(const QByteArray &input, const Utils::FilePath &workingDirectory = {},
                         int strip = 0, bool reverse = false);
};

} // namespace Core
