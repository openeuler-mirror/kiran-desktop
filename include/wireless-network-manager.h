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
#include <NetworkManagerQt/Device>
#include <NetworkManagerQt/WirelessDevice>
#include <QList>
#include <QMap>
#include <QObject>
#include "network-common.h"

namespace Kiran
{
class WirelessNetworkManagerPrivate;
typedef QList<WifiSecurityType> WifiSecurityTypeList;
class Q_DECL_EXPORT WirelessNetworkManager : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(WirelessNetworkManager);

public:
    // NetworkManager::WirelessDevice::Ptr请使用NetworkManager::findNetworkInterface所构造的对象
    // 避免多次创建对象，导致信号处理时数据不同步问题
    WirelessNetworkManager(const NetworkManager::WirelessDevice::Ptr &device,
                           QObject *parent = nullptr);
    ~WirelessNetworkManager() override;

    NetworkManager::Device::State state() const;
    QString uni() const;
    QString interfaceName() const;
    QString activatedNetowrk() const;

    void requestScan();

    // 获取网络列表/网络信息
    WirelessNetworkInfoList getNetworkInfoList();
    WirelessNetworkInfo getNetworkInfo(const QString &ssid);

    // 检查网络能否直连
    bool checkNetworkCanDirectConn(const QString &ssid);

    // 获取网络安全类型
    WifiSecurityType networkBestSecurityType(const QString &ssid);
    WifiSecurityTypeList networkSupportSecurityTypeList(const QString &ssid);

    // 激活网络,使用已有的网络配置
    void activateNetowrk(const QString &ssid);

    // 添加密码形式激活网络配置并尝试激活隐藏网络
    // 适用于WpaPsk/Wpa2Psk/Wpa3 Person/WpaNone
    void addAndActivateHiddenNetwork(const QString &ssid, const QString &password, WifiSecurityType securityType);

    // 添加密码形式激活网的配置并尝试激活网络
    // 适用于WpaPsk/Wpa2Psk/Wpa3 Person/WpaNone
    void addAndActivateNetwork(const QString &ssid, const QString &password);

    // 添加EAP形式激活网的配置并尝试激活网络
    // 适用于Wpa/Wpa2 Enterprise
    // void addAndActivateNetworkByEAP(const QString &ssid, const QString &password);

    // 删除指定无线网络的已有配置(忘记已知网络)
    void removeNetworkConnection(const QString &ssid);

    // 断开该无线网卡上已激活的连接
    void deactivateConnection();

signals:
    // 设备状态改变信号
    void stateChanged(NetworkManager::Device::State state);
    // 无线网络出现/消失
    void networkAppeared(const QString &ssid);
    void networkDisappeared(const QString &ssid);
    // 当前激活的无线网络改变信号
    void activeNetworkChanged(const QString &ssid);
    // 扫描结束 (kf5-networkmanager-qt > 5.62.0 && nm > 1.22)
    void scanFinished();

private:
    WirelessNetworkManagerPrivate *const d_ptr;
};
};  // namespace Kiran