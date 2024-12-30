/**
 * Copyright (c) 2020 ~ 2024 KylinSec Co., Ltd.
 * network is licensed under Mulan PSL v2.
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
#pragma once
#include <QString>
#include <QList>
#include <QMap>
#include <QDebug>
namespace Kiran
{
    typedef struct WirelessNetworkInfo
    {
        WirelessNetworkInfo(QString _ssid, QString _referencePointPath, int _signalStreagth);
        WirelessNetworkInfo();
        QString ssid;
        QString referencePointPath;
        int signalStreagth;
    } WirelessNetworkInfo;
    typedef QList<WirelessNetworkInfo> WirelessNetworkInfoList;
    typedef QMap<QString, WirelessNetworkInfo> WirelessNetworkInfoMap;
    QDebug operator<<(QDebug &debug, const WirelessNetworkInfo &info);
    QDebug operator<<(QDebug &debug, const QList<WirelessNetworkInfo> &list);

    enum WifiSecurityType
    {
        SECURITY_TYPE_NONE,
        SECURITY_TYPE_WPA_AND_WPA2_PERSON,
        SECURITY_TYPE_WPA_AND_WPA2_ENTERPRISE,
        SECURITY_TYPE_WPA3_PERSON,
        SECURITY_TYPE_UNKNOWN
    };
}