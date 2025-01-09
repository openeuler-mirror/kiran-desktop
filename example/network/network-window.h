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
#pragma once
#include <QWidget>
#include <NetworkManagerQt/Device>

namespace Ui
{
class NetworkWindow;
}

namespace Kiran
{
    class WirelessNetworkManager;
    class NMSecretAgent;
}

class QListWidget;
class QListWidgetItem;
class QLabel;
class NetworkWindow : public QWidget
{
    Q_OBJECT
public:
    explicit NetworkWindow(QWidget *parent = nullptr);
    ~NetworkWindow();

private:
    void init();
    void initNetworkList();
    void addNetworkItem(const QString &ssid, int streagth, int position = -1);

private slots:
    void onNetworkAppeared(const QString &ssid);
    void onNetworkDisappeared(const QString &ssid);
    void onActivateNetworkChanged(const QString& ssid);
    void onItemActivated(QListWidgetItem *item);
    void onDeviceStateChagned(NetworkManager::Device::State state);
    void onSecretAgentRequsetPasswd(const QString &devicePath,
                                    const QString &ssid,
                                    bool wait);

private:
    Ui::NetworkWindow* ui = nullptr;
    QString m_wirelessDevicePath;
    Kiran::WirelessNetworkManager *m_networkManager = NULL;
    Kiran::NMSecretAgent *m_secretAgent = NULL;
};