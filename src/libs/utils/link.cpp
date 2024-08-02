#include "link.h"

#include "linecolumn.h"

namespace Utils {

/*!
    Returns the Link to \a fileName.
    If \a canContainLineNumber is true the line number, and column number components
    are extracted from \a fileName and the found \a postfix is set.

    The following patterns are supported: \c {filepath.txt:19},
    \c{filepath.txt:19:12}, \c {filepath.txt+19},
    \c {filepath.txt+19+12}, and \c {filepath.txt(19)}.
*/
Link Link::fromString(const QString &fileName, bool canContainLineNumber, QString *postfix)
{
    if (!canContainLineNumber)
        return {FilePath::fromString(fileName)};
    int postfixPos = -1;
    const LineColumn lineColumn = LineColumn::extractFromFileName(fileName, postfixPos);
    if (postfix && postfixPos >= 0)
        *postfix = fileName.mid(postfixPos);
    return {FilePath::fromString(fileName.left(postfixPos)),
            lineColumn.line,
            lineColumn.column};
}

Link Link::fromFilePath(const FilePath &filePath, bool canContainLineNumber, QString *postfix)
{
    if (!canContainLineNumber)
        return {filePath};
    int postfixPos = -1;
    QString fileName = filePath.path();
    const LineColumn lineColumn = LineColumn::extractFromFileName(fileName, postfixPos);
    if (postfix && postfixPos >= 0)
        *postfix = fileName.mid(postfixPos);
    return Link{filePath.withNewPath(fileName.left(postfixPos)), lineColumn.line, lineColumn.column};
}

QHashValueType qHash(const Link &l)
{
    QString s = l.targetFilePath.toString();
    return qHash(s.append(':').append(QString::number(l.targetLine)).append(':')
                 .append(QString::number(l.targetColumn)));
}

} // namespace Utils
