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
#include "network-window.h"
#include "nm-secret-agent.h"
#include "ui_network-window.h"
#include "wireless-network-manager.h"

#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/WirelessDevice>
#include <QDebug>

#include <QBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QListWidget>
#include <QMetaEnum>
#include <QPushButton>

#define NETWORK_ITEM_SSID_ROLE Qt::UserRole + 1
#define NETWORK_ITEM_SIGNAL_SIGNAL_STREAGTH Qt::UserRole + 2

using namespace Kiran;

NetworkWindow::NetworkWindow(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::NetworkWindow)
{
    ui->setupUi(this);
    init();
    initNetworkList();
}

NetworkWindow::~NetworkWindow()
{
}

void NetworkWindow::init()
{
    m_secretAgent = new NMSecretAgent(this);

    // 获取无线网络列表
    const auto deviceList = NetworkManager::networkInterfaces();
    for (auto device : deviceList)
    {
        auto deviceType = device->type();
        if (deviceType != NetworkManager::Device::Wifi)
        {
            continue;
        }
        auto wirelessDevice = device.objectCast<NetworkManager::WirelessDevice>();
        m_wirelessDevicePath = wirelessDevice->uni();
        m_networkManager = new Kiran::WirelessNetworkManager(wirelessDevice, this);
        qInfo() << "using wireless device:"
                << wirelessDevice->interfaceName()
                << wirelessDevice->uni();
        break;
    }

    // 请求扫描
    if (m_networkManager)
    {
        m_networkManager->requestScan();
    }

    // clang-format off
    // 刷新网络
    connect(ui->btn_refresh, &QPushButton::clicked, this, [this](){ 
        this->m_networkManager->requestScan(); 
    });
    
    // 忽略/忘记网络配置
    connect(ui->btn_ignore, &QPushButton::clicked, this, [this](){
        auto items = ui->list_network->selectedItems();
        for(auto  item : items )
        {
            auto ssid = item->data(NETWORK_ITEM_SSID_ROLE).toString();
            m_networkManager->removeNetworkConnection(ssid);
        } 
    });

    connect(ui->btn_disconnect,&QPushButton::clicked,this,[this](){
        m_networkManager->deactivateConnection();
    });
    // clang-format on

    // 当前激活的无线网络信息展示
    onActivateNetworkChanged(m_networkManager->activatedNetowrk());
    connect(m_networkManager, &WirelessNetworkManager::activeNetworkChanged,
            this, &NetworkWindow::onActivateNetworkChanged);

    connect(ui->list_network, &QListWidget::itemActivated, this, &NetworkWindow::onItemActivated);

    // 连接SecretAgent请求密码，弹出密码框
    connect(m_secretAgent, &NMSecretAgent::requestPassword,
            this, &NetworkWindow::onSecretAgentRequsetPasswd);
}

void NetworkWindow::initNetworkList()
{
    if (!m_networkManager)
    {
        return;
    }

    auto networkList = m_networkManager->getNetworkInfoList();
    for (auto network : networkList)
    {
        addNetworkItem(network.ssid, network.signalStreagth, -1);
    }

    connect(m_networkManager, &Kiran::WirelessNetworkManager::networkAppeared,
            this, &NetworkWindow::onNetworkAppeared);
    connect(m_networkManager, &Kiran::WirelessNetworkManager::stateChanged,
            this, &NetworkWindow::onDeviceStateChagned);
    connect(m_networkManager, &Kiran::WirelessNetworkManager::networkDisappeared,
            this, &NetworkWindow::onNetworkDisappeared);

    onDeviceStateChagned(m_networkManager->state());
}

void NetworkWindow::addNetworkItem(const QString &ssid, int streagth, int position)
{
    auto item = new QListWidgetItem(ui->list_network);

    QString desc = QString("%1 streagth:%2").arg(ssid).arg(streagth);
    item->setText(desc);

    item->setData(NETWORK_ITEM_SSID_ROLE, ssid);
    item->setData(NETWORK_ITEM_SIGNAL_SIGNAL_STREAGTH, streagth);

    if (position == -1)
    {
        ui->list_network->addItem(item);
    }
    else
    {
        ui->list_network->insertItem(position, item);
    }
}

void NetworkWindow::onNetworkAppeared(const QString &ssid)
{
    auto networkItem = m_networkManager->getNetworkInfo(ssid);
    addNetworkItem(networkItem.ssid, networkItem.signalStreagth);
}

void NetworkWindow::onNetworkDisappeared(const QString &ssid)
{
    for (int i = 0; i < ui->list_network->count();)
    {
        auto item = ui->list_network->item(i);
        auto itemSsid = item->data(NETWORK_ITEM_SSID_ROLE).toString();
        if (ssid == itemSsid)
        {
            ui->list_network->takeItem(i);
            continue;
        }
        i++;
    }
}

void NetworkWindow::onActivateNetworkChanged(const QString &ssid)
{
    ui->label_activatedNetwork->setText(ssid);
}

void NetworkWindow::onItemActivated(QListWidgetItem *item)
{
    // 检查网络能否直接连接(存在已有配置)
    auto ssid = item->data(NETWORK_ITEM_SSID_ROLE).toString();
    auto canDirectConn = m_networkManager->checkNetworkCanDirectConn(ssid);
    if (canDirectConn)
    {
        m_networkManager->activateNetowrk(ssid);
        qInfo() << "activate network" << ssid;
        return;
    }

    // 检查网络最佳安全类型,暂时只做密码认证
    auto bestSecurityType = m_networkManager->networkBestSecurityType(ssid);

    QString passwd;
    bool passwdAuth = (bestSecurityType == SECURITY_TYPE_WPA_AND_WPA2_PERSON) || (bestSecurityType == SECURITY_TYPE_WPA3_PERSON);

    if (bestSecurityType == SECURITY_TYPE_NONE)
    {
        qInfo() << "wirelesss network" << ssid << "no security type";
    }
    else if (passwdAuth)
    {
        // 请求输入密码
        bool isOk = false;
        QString label = QString("WI-FI(%1) requires password").arg(ssid);
        passwd = QInputDialog::getText(this, "Password", label, QLineEdit::PasswordEchoOnEdit, "", &isOk);
        if (!isOk || passwd.isEmpty())
        {
            qInfo() << "cancel input password";
            return;
        }
    }
    else
    {
        qInfo() << "wirelesss network" << ssid << "unknown security type";
        return;
    }

    // 添加配置并激活
    m_networkManager->addAndActivateNetwork(ssid, passwd);
}

void NetworkWindow::onDeviceStateChagned(NetworkManager::Device::State state)
{
    QMetaEnum metaEnum = QMetaEnum::fromType<NetworkManager::Device::State>();
    QString stateString = metaEnum.valueToKey(state);
    ui->label_state->setText(stateString);
}

void NetworkWindow::onSecretAgentRequsetPasswd(const QString &devicePath,
                                               const QString &ssid, bool wait)
{
    bool isOK = false;
    qWarning() << "wirelesss network" << ssid << "request passwd";

    QString label = QString("WI-FI(%1) requires password re-entry").arg(ssid);
    QString passwd = QInputDialog::getText(this, "Password", label, QLineEdit::PasswordEchoOnEdit, "", &isOK);

    m_secretAgent->respondPasswdRequest(ssid, passwd, !isOK);
}
