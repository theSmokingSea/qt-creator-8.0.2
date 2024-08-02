#pragma once

#include "utils_global.h"

#include "filepath.h"

#include <QCoreApplication>

namespace Utils {

class Environment;

class QTCREATOR_UTILS_EXPORT BuildableHelperLibrary
{
    Q_DECLARE_TR_FUNCTIONS(ProjectExplorer::DebuggingHelperLibrary)

public:
    // returns the full path to the first qmake, qmake-qt4, qmake4 that has
    // at least version 2.0.0 and thus is a qt4 qmake
    static FilePath findSystemQt(const Environment &env);
    static FilePaths findQtsInEnvironment(const Environment &env, int maxCount = -1);
    static bool isQtChooser(const FilePath &filePath);
    static FilePath qtChooserToQmakePath(const FilePath &path);
    // return true if the qmake at qmakePath is a Qt (used by QtVersion)
    static QString qtVersionForQMake(const FilePath &qmakePath);
    // returns something like qmake4, qmake, qmake-qt4 or whatever distributions have chosen (used by QtVersion)
    static QStringList possibleQMakeCommands();
    static QString filterForQmakeFileDialog();
};

} // Utils
