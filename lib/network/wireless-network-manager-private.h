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
#include <NetworkManagerQt/Utils>
#include <NetworkManagerQt/WirelessDevice>
#include <QObject>
#include "wireless-network-manager.h"

#define DBUS_WATCHER_PROPERTY_SSID "dbus_watch_property_ssid"

using namespace NetworkManager;

namespace Kiran
{
class WirelessNetworkManagerPrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(WirelessNetworkManager)
public:
    WirelessNetworkManagerPrivate(WirelessNetworkManager *ptr,
                                  const WirelessDevice::Ptr &wirelessDevice,
                                  QObject *parent);
    void init();
    void loadNetworkInfoList();

private slots:
    void onNetworkAppeared(const QString &ssid);
    void onNetworkDisappeared(const QString &ssid);
    void onStateChanged(Device::State newstate, Device::State oldstate, Device::StateChangeReason reason);
    void onActiveAccessPointChanged(const QString &ap);
    void onDeviceActiveConnectionChanged();

private:
    /* 缓存中添加/删除网络 */
    bool addNetwork(const QString &ssid, bool isInit = false);
    bool removeNetwork(const QString &ssid);

    WifiSecurityType convertNMSecurityType(WirelessSecurityType securityType);
    QList<WirelessSecurityType> getAPSecuritySupport(const QString &ap);
    WifiSecurityType getBestNetworkSecurity(const QString &ssid);
    WifiSecurityTypeList getSupportedNetworkSecuritys(const QString &ssid);
    Connection::List getWirelessNetworkConnection(const QString &ssid);
    ConnectionSettings::Ptr createConnectionSettings(const QString &ssid,
                                                     WifiSecurityType securityType,
                                                     const QString &password,
                                                     bool hidden = false);

private:
    WirelessNetworkManager *q_ptr;
    WirelessDevice::Ptr m_device;
    quint64 m_timeScanInterval = 0;
    WirelessNetworkInfoMap m_networkInfoMap;
};
}  // namespace Kiran