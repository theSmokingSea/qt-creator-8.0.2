#pragma once

#include <QWidget>

namespace Core {
namespace Internal {

class EditorArea;

class EditorWindow : public QWidget
{
    Q_OBJECT
public:
    explicit EditorWindow(QWidget *parent = nullptr);
    ~EditorWindow() override;

    EditorArea *editorArea() const;

    QVariantHash saveState() const;
    void restoreState(const QVariantHash &state);
private:
    void updateWindowTitle();

    EditorArea *m_area;
};

} // Internal
} // Core
