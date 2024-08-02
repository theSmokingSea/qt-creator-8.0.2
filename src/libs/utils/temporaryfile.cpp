#include "temporaryfile.h"

#include "temporarydirectory.h"
#include "qtcassert.h"

namespace Utils {

TemporaryFile::TemporaryFile(const QString &pattern) :
    QTemporaryFile(TemporaryDirectory::masterTemporaryDirectory()->path() + '/' + pattern)
{
    QTC_CHECK(!QFileInfo(pattern).isAbsolute());
}

TemporaryFile::~TemporaryFile() = default;

} // namespace Utils
