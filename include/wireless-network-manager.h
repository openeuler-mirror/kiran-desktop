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
#include <QObject>
#include <NetworkManagerQt/Device>
#include <NetworkManagerQt/WirelessDevice>
#include <NetworkManagerQt/WirelessSetting>
#include <NetworkManagerQt/WirelessSecuritySetting>
#include <QList>
#include <QMap>
#include "network-common.h"

/**
 * @brief 无线网络管理类
 */
namespace Kiran
{
    class WirelessNetworkManagerPrivate;
    typedef QList<WifiSecurityType> WifiSecurityTypeList;
    class Q_DECL_EXPORT WirelessNetworkManager : public QObject
    {
        Q_OBJECT
        Q_DECLARE_PRIVATE(WirelessNetworkManager);

    public:
        WirelessNetworkManager(const NetworkManager::WirelessDevice::Ptr &device,
                               QObject *parent = nullptr);
        WirelessNetworkManager(const QString &devicePath,
                               QObject *parent = nullptr);
        ~WirelessNetworkManager() override;

        NetworkManager::Device::State state() const;
        QString uni() const;
        QString activateNetowrk() const;

        void requestScan();

        WirelessNetworkInfoList getNetworkInfoList();
        WirelessNetworkInfo getNetworkInfo(const QString &ssid);

        bool checkNetworkCanDirectConn(const QString &ssid);
        WifiSecurityType networkBestSecurityType(const QString &ssid);
        WifiSecurityTypeList networkSupportSecurityTypeList(const QString &ssid);

        /* 删除指定无线网络的已有配置(忘记已知网络) */
        void removeNetworkConnection(const QString &ssid);

        /* 激活网络,使用已有的网络配置 */
        void activateNetowrk(const QString &ssid);
        /* 添加密码形式激活网的配置并尝试激活隐藏网络，适用于WpaPsk/Wpa2Psk/Wpa3 Person/WpaNone */
        void addAndActivateHiddenNetwork(const QString &ssid, const QString &password, WifiSecurityType securityType);
        /* 添加密码形式激活网的配置并尝试激活网络，适用于WpaPsk/Wpa2Psk/Wpa3 Person/WpaNone */
        void addAndActivateNetwork(const QString &ssid, const QString &password);
        /* 添加EAP形式激活网的配置并尝试激活网络，适用于Wpa/Wpa2 Enterprise */
        // void addAndActivateNetworkByEAP(const QString &ssid, const QString &password);

    signals:
        void stateChanged(NetworkManager::Device::State state);
        void networkAppeared(const QString &ssid);
        void networkDisappeared(const QString &ssid);
        /* 扫描结束 (kf5-networkmanager-qt > 5.62.0 && nm > 1.22) */
        void scanFinished();

    private:
        WirelessNetworkManagerPrivate *const d_ptr;
    };
};