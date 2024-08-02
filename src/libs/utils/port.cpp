#include "port.h"

#include "stringutils.h"

/*! \class Utils::Port

  \brief The Port class implements a wrapper around a 16 bit port number
  to be used in conjunction with IP addresses.
*/

namespace Utils {

QList<Port> Port::parseFromSedOutput(const QByteArray &output)
{
    QList<Port> ports;
    const QList<QByteArray> portStrings = output.split('\n');
    for (const QByteArray &portString : portStrings) {
        if (portString.size() != 4)
            continue;
        bool ok;
        const Port port(portString.toInt(&ok, 16));
        if (ok) {
            if (!ports.contains(port))
                ports << port;
        } else {
            qWarning("%s: Unexpected string '%s' is not a port.",
                     Q_FUNC_INFO, portString.data());
        }
    }
    return ports;
}

QList<Port> Port::parseFromNetstatOutput(const QByteArray &output)
{
    QList<Port> ports;
    const QList<QByteArray> lines = output.split('\n');
    for (const QByteArray &line : lines) {
        const Port port(parseUsedPortFromNetstatOutput(line));
        if (port.isValid() && !ports.contains(port))
            ports.append(port);
    }
    return ports;
}

} // Utils
