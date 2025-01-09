#include "network-common.h"

namespace Kiran
{
    QDebug operator<<(QDebug &debug, const WirelessNetworkInfo &info)
    {
        debug.nospace() << "WirelessNetworkInfo(";
        debug.nospace() << "ssid:" << info.ssid;
        debug.nospace() << ",referencePointPath:" << info.referencePointPath;
        debug.nospace() << ",signalStreagth:" << info.signalStreagth;
        debug.nospace() << ")";
        return debug;
    };

    QDebug operator<<(QDebug &debug, const QList<WirelessNetworkInfo> &list)
    {
        debug.nospace() << "QList<WirelessNetworkInfo>(" << Qt::endl;
        for (auto info : list)
        {
            debug << '\t' << info << Qt::endl;
        }
        debug.nospace() << ")" << Qt::endl;
        return debug;
    };

    WirelessNetworkInfo::WirelessNetworkInfo(QString _ssid, QString _referencePointPath, int _signalStreagth)
        : ssid(_ssid), referencePointPath(_referencePointPath),
          signalStreagth(_signalStreagth)
    {
    }
    WirelessNetworkInfo::WirelessNetworkInfo() {
    };
}
