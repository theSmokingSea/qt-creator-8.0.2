#pragma once

#include "utils_global.h"

#include "namevaluemodel.h"

namespace Utils {

class QTCREATOR_UTILS_EXPORT EnvironmentModel : public NameValueModel
{
    Q_OBJECT

public:
    Environment baseEnvironment() const;
    void setBaseEnvironment(const Environment &env);
};

} // namespace Utils
