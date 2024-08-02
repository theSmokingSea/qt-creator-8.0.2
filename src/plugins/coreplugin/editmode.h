#pragma once

#include <coreplugin/imode.h>

QT_BEGIN_NAMESPACE
class QSplitter;
class QVBoxLayout;
QT_END_NAMESPACE

namespace Core {

class EditorManager;

namespace Internal {

class EditMode : public IMode
{
    Q_OBJECT

public:
    EditMode();
    ~EditMode() override;

private:
    void grabEditorManager(Utils::Id mode);

    QSplitter *m_splitter;
    QVBoxLayout *m_rightSplitWidgetLayout;
};

} // namespace Internal
} // namespace Core
