#include <utils/qtcassert.h>

#include "newdialog.h"

using namespace Core;

NewDialog::NewDialog()
{
    QTC_CHECK(m_currentDialog == nullptr);

    m_currentDialog = this;
}

NewDialog::~NewDialog()
{
    QTC_CHECK(m_currentDialog != nullptr);
    m_currentDialog = nullptr;
}
