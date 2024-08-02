#pragma once

#include "ioutputpane.h"

namespace Core {
class OutputWindow;

namespace Internal {

class MessageOutputWindow : public IOutputPane
{
    Q_OBJECT

public:
    MessageOutputWindow();
    ~MessageOutputWindow() override;

    QWidget *outputWidget(QWidget *parent) override;

    QString displayName() const override;
    int priorityInStatusBar() const override;
    void clearContents() override;

    void append(const QString &text);
    bool canFocus() const override;
    bool hasFocus() const override;
    void setFocus() override;

    bool canNext() const override;
    bool canPrevious() const override;
    void goToNext() override;
    void goToPrev() override;
    bool canNavigate() const override;

private:
    void updateFilter() override;

    OutputWindow *m_widget;
};

} // namespace Internal
} // namespace Core
