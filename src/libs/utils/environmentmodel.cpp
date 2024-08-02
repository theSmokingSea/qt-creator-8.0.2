#include "environmentmodel.h"

#include "environment.h"

namespace Utils {

Environment EnvironmentModel::baseEnvironment() const
{
    return Environment(baseNameValueDictionary());
}

void EnvironmentModel::setBaseEnvironment(const Environment &env)
{
    setBaseNameValueDictionary(env.toDictionary());
}

} // namespace Utils
