#pragma once

#include "editorview.h"

#include <QPointer>

namespace Core {

class IContext;

namespace Internal {

class EditorArea : public SplitterOrView
{
    Q_OBJECT

public:
    EditorArea();
    ~EditorArea() override;

    IDocument *currentDocument() const;

signals:
    void windowTitleNeedsUpdate();

private:
    void focusChanged(QWidget *old, QWidget *now);
    void setCurrentView(EditorView *view);
    void updateCurrentEditor(IEditor *editor);
    void updateCloseSplitButton();

    IContext *m_context;
    QPointer<EditorView> m_currentView;
    QPointer<IDocument> m_currentDocument;
};

} // Internal
} // Core
