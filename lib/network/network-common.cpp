/**
 * Copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 * kiran-desktop is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     liuxinhao <liuxinhao@kylinsec.com.cn>
 */
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
    : ssid(_ssid), referencePointPath(_referencePointPath), signalStreagth(_signalStreagth)
{
}
WirelessNetworkInfo::WirelessNetworkInfo() {
};
}  // namespace Kiran
