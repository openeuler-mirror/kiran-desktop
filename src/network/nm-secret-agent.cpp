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
#include "nm-secret-agent.h"
#include "nm-secret-agent-private.h"
#include "logging-category.h"

#include <QDBusConnection>
#include <QStringBuilder>
#include <QtDBus/qdbusmetatype.h>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QTimer>

#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/GenericTypes>
#include <NetworkManagerQt/GsmSetting>
#include <NetworkManagerQt/Security8021xSetting>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/VpnSetting>
#include <NetworkManagerQt/WirelessSecuritySetting>
#include <NetworkManagerQt/WirelessSetting>

#define DEBUG_PRINT qDebug()
using namespace NetworkManager;

namespace Kiran
{
    NMSecretAgent::NMSecretAgent(QObject *parent)
        : NetworkManager::SecretAgent(QStringLiteral("com.kylinsec.Kiran.NMSecretAgent"), parent),
          d_ptr(new NMSecretAgentPrivate(this))
    {
        qDBusRegisterMetaType<NMVariantMapMap>();
    }

    NMSecretAgent::~NMSecretAgent() = default;

    NMVariantMapMap NMSecretAgent::GetSecrets(const NMVariantMapMap &connection,
                                              const QDBusObjectPath &connection_path,
                                              const QString &setting_name,
                                              const QStringList &hints,
                                              uint flags)
    {
        const QString callId = connection_path.path() % setting_name;
        for (const SecretsRequest &request : d_ptr->m_calls)
        {
            if (request == callId)
            {
                KLOG_WARNING(qLcNetworkSecretAgent) << "GetSecrets was called again! This should not happen,"
                                                       " cancelling first call"
                                                    << connection_path.path()
                                                    << setting_name;
                CancelGetSecrets(connection_path, setting_name);
                break;
            }
        }

        setDelayedReply(true);

        SecretsRequest request(SecretsRequest::GetSecrets);
        request.callId = callId;
        request.connection = connection;
        request.connection_path = connection_path;
        request.flags = static_cast<NetworkManager::SecretAgent::GetSecretsFlags>(flags);
        request.hints = hints;
        request.setting_name = setting_name;
        request.message = message();

        auto connectionSettings = ConnectionSettings::Ptr::create(connection);
        if (connectionSettings->connectionType() == NetworkManager::ConnectionSettings::Wireless)
        {
            auto wirelessSetting = connectionSettings->setting(Setting::Wireless).dynamicCast<WirelessSetting>();
            request.ssid = wirelessSetting->ssid();
        }
        d_ptr->m_calls << request;

        KLOG_DEBUG(qLcNetworkSecretAgent).nospace() << "get secrets for wireless network"
                                                    << request.callId << ", " << request.ssid
                                                    << "(" << connection_path << ")"
                                                    << " ,hints(" << hints.join(',') << ")"
                                                    << " ,flags(" << flags << ")";

        d_func()->processNext();
        return {};
    }

    void NMSecretAgent::SaveSecrets(const NMVariantMapMap &connection, const QDBusObjectPath &connection_path)
    {
        KLOG_INFO(qLcNetworkSecretAgent) << "save secrets for" << connection_path.path();

        setDelayedReply(true);
        SecretsRequest::Type type;
        if (d_func()->hasSecrets(connection))
        {
            type = SecretsRequest::SaveSecrets;
        }
        else
        {
            type = SecretsRequest::DeleteSecrets;
        }

        SecretsRequest request(type);
        request.connection = connection;
        request.connection_path = connection_path;
        request.message = message();
        d_ptr->m_calls << request;

        d_func()->processNext();
    }

    void NMSecretAgent::DeleteSecrets(const NMVariantMapMap &connection, const QDBusObjectPath &connection_path)
    {
        KLOG_INFO(qLcNetworkSecretAgent) << "delete sercres for" << connection_path.path();

        setDelayedReply(true);
        SecretsRequest request(SecretsRequest::DeleteSecrets);
        request.connection = connection;
        request.connection_path = connection_path;
        request.message = message();
        d_ptr->m_calls << request;

        d_func()->processNext();
    }

    void NMSecretAgent::CancelGetSecrets(const QDBusObjectPath &connection_path, const QString &setting_name)
    {
        KLOG_INFO(qLcNetworkSecretAgent).nospace() << "cancel conn("
                                                   << connection_path.path()
                                                   << ") GetSecrets";

        QString callId = connection_path.path() % setting_name;
        for (int i = 0; i < d_ptr->m_calls.size(); ++i)
        {
            SecretsRequest request = d_ptr->m_calls.at(i);
            if (request.type == SecretsRequest::GetSecrets && callId == request.callId)
            {
                if (d_ptr->m_requestedPasswordSsid == request.ssid)
                {
                    KLOG_WARNING(qLcNetworkSecretAgent) << "process finished (agent canceled)";
                    d_ptr->m_requestedPasswordSsid.clear();
                }
                sendError(NMSecretAgent::AgentCanceled, QStringLiteral("Agent canceled the password dialog"), request.message);
                d_ptr->m_calls.removeAt(i);
                break;
            }
        }

        d_func()->processNext();
    }

    void NMSecretAgent::respondPasswdRequest(const QString &ssid, const QString &password, bool isCancel)
    {
        KLOG_INFO(qLcNetworkSecretAgent) << "password inputed, ssid:" << ssid << "cancel:" << isCancel;

        if (ssid != d_ptr->m_requestedPasswordSsid)
        {
            KLOG_WARNING(qLcNetworkSecretAgent) << "inconsistency between reply ssid and request ssid";
            return;
        }

        for (int i = 0; i < d_ptr->m_calls.size(); ++i)
        {
            SecretsRequest &request = d_ptr->m_calls[i];
            if (request.type != SecretsRequest::GetSecrets || request.ssid != ssid)
            {
                continue;
            }

            auto connectionSettings = NetworkManager::ConnectionSettings::Ptr::create(request.connection);
            NetworkManager::Setting::Ptr setting = connectionSettings->setting(request.setting_name);
            auto needSecrets = setting->needSecrets(request.flags & RequestNew);

            if (isCancel || password.isEmpty())
            {
                KLOG_INFO(qLcNetworkSecretAgent) << "cancel GetSecrets request" << ssid << ", user canceled";
                sendError(NMSecretAgent::UserCanceled, QStringLiteral("user canceled"), request.message);
            }
            else if (needSecrets.isEmpty())
            {
                sendError(NMSecretAgent::InternalError, QStringLiteral("No Secrets were requested"), request.message);
            }
            else
            {
                QVariantMap result;
                KLOG_INFO(qLcNetworkSecretAgent) << request.callId << "fill" << ssid << "secret" << needSecrets.first();
                result.insert(needSecrets.first(), password);
                request.connection[request.setting_name] = result;
                d_func()->sendSecrets(request.connection, request.message);
            }

            d_ptr->m_requestedPasswordSsid.clear();
            d_ptr->m_calls.removeAt(i);
            break;
        }
    }

    void NMSecretAgentPrivate::processNext()
    {
        int i = 0;
        while (i < m_calls.size())
        {
            SecretsRequest &request = m_calls[i];
            switch (request.type)
            {
            case SecretsRequest::GetSecrets:
                if (processGetSecrets(request))
                {
                    m_calls.removeAt(i);
                    continue;
                }
                break;
            case SecretsRequest::SaveSecrets:
                if (processSaveSecrets(request))
                {
                    m_calls.removeAt(i);
                    continue;
                }
                break;
            case SecretsRequest::DeleteSecrets:
                if (processDeleteSecrets(request))
                {
                    m_calls.removeAt(i);
                    continue;
                }
                break;
            }
            ++i;
        }
    }

    bool NMSecretAgentPrivate::processGetSecrets(SecretsRequest &request)
    {
        // 若已有请求密码正在处理,则先不处理GetSecrets请求
        if (!m_requestedPasswordSsid.isEmpty())
        {
            return false;
        }

        auto connectionSettings = NetworkManager::ConnectionSettings::Ptr::create(request.connection);
        NetworkManager::Setting::Ptr setting = connectionSettings->setting(request.setting_name);

        const bool requestNew = request.flags & SecretAgent::RequestNew;
        const bool userRequested = request.flags & SecretAgent::UserRequested;
        const bool allowInteraction = request.flags & SecretAgent::AllowInteraction;
        const bool isVpn = (setting->type() == NetworkManager::Setting::Vpn);
        const bool isWifi = (connectionSettings->connectionType() == NetworkManager::ConnectionSettings::Wireless);

        bool removeRequest = true;
        if (isVpn)
        {
            // pick from plasma-nm
            auto vpnSetting = connectionSettings->setting(NetworkManager::Setting::Vpn).dynamicCast<NetworkManager::VpnSetting>();
            if (vpnSetting->serviceType() == QLatin1String("org.freedesktop.NetworkManager.ssh") && vpnSetting->data()["auth-type"] == QLatin1String("ssh-agent"))
            {
                QString authSock = qgetenv("SSH_AUTH_SOCK");
                KLOG_DEBUG(qLcNetworkSecretAgent) << "get secrets,sending SSH auth socket" << authSock;

                if (authSock.isEmpty())
                {
                    q_func()->sendError(NMSecretAgent::NoSecrets, QStringLiteral("SSH_AUTH_SOCK not present"), request.message);
                }
                else
                {
                    NMStringMap secrets;
                    secrets.insert(QStringLiteral("ssh-auth-sock"), authSock);

                    QVariantMap secretData;
                    secretData.insert(QStringLiteral("secrets"), QVariant::fromValue<NMStringMap>(secrets));
                    request.connection[request.setting_name] = secretData;
                    sendSecrets(request.connection, request.message);
                }
            }
            else
            {
                q_func()->sendError(SecretAgent::InternalError, QStringLiteral("did not know how to handle the request"), request.message);
            }
        }
        else if (isWifi)
        {
            if (userRequested && (requestNew || allowInteraction) && !setting->needSecrets(requestNew).isEmpty())
            {
                KLOG_WARNING(qLcNetworkSecretAgent).nospace() << "handles requests for wireless network"
                                                              << "(" << request.connection_path << ',' << request.callId << ")";
                auto wirelessSetting = connectionSettings->setting(Setting::Wireless).dynamicCast<WirelessSetting>();

                QString devPath;
                auto activeConns = NetworkManager::activeConnections();
                for (auto it = activeConns.begin(); it != activeConns.end(); ++it)
                {
                    if ((*it)->connection()->path() == request.connection_path.path())
                    {
                        devPath = (*it)->devices().first();
                        break;
                    }
                }

                m_requestedPasswordSsid = wirelessSetting->ssid();
                auto ssid = request.ssid;

                // 延迟发送，先等处理结束
                QTimer::singleShot(0, this, [this, devPath, ssid]()
                                   { Q_EMIT q_func() -> requestPassword(devPath, ssid, true); });
                KLOG_INFO(qLcNetworkSecretAgent) << "request passwd for" << m_requestedPasswordSsid;
                
                // 保留请求记录后续接受用户输入再回复
                removeRequest = false;
            }
            else
            {
                KLOG_WARNING() << request.callId << request.ssid << "did not know how hto handle the request";
                q_func()->sendError(SecretAgent::InternalError, QStringLiteral("did not know how to handle the request"), request.message);
            }
        }
        else
        {
            q_func()->sendError(SecretAgent::InternalError, QStringLiteral("did not know how to handle the request"), request.message);
        }

        return removeRequest;
    }

    bool NMSecretAgentPrivate::processSaveSecrets(SecretsRequest &request) const
    {
        if (!request.saveSecretsWithoutReply)
        {
            QDBusMessage reply = request.message.createReply();
            if (!QDBusConnection::systemBus().send(reply))
            {
                KLOG_WARNING(qLcNetworkSecretAgent) << "failed send SaveSecrets Reply into the queue";
            }
        }

        return true;
    }

    bool NMSecretAgentPrivate::processDeleteSecrets(SecretsRequest &request) const
    {
        QDBusMessage reply = request.message.createReply();
        if (!QDBusConnection::systemBus().send(reply))
        {
            KLOG_WARNING(qLcNetworkSecretAgent) << "failed put delete secrets reply into the queue";
        }

        return true;
    }

    bool NMSecretAgentPrivate::hasSecrets(const NMVariantMapMap &connection) const
    {
        NetworkManager::ConnectionSettings connectionSettings(connection);
        for (const NetworkManager::Setting::Ptr &setting : connectionSettings.settings())
        {
            if (!setting->secretsToMap().isEmpty())
            {
                return true;
            }
        }

        return false;
    }

    void NMSecretAgentPrivate::sendSecrets(const NMVariantMapMap &secrets, const QDBusMessage &message) const
    {
        QDBusMessage reply;
        reply = message.createReply(QVariant::fromValue(secrets));
        if (!QDBusConnection::systemBus().send(reply))
        {
            KLOG_WARNING(qLcNetworkSecretAgent) << "failed put the secret into the queue";
        }
    }

    NMSecretAgentPrivate::NMSecretAgentPrivate(NMSecretAgent *ptr)
        : QObject(ptr),
          q_ptr(ptr)
    {
    }

    NMSecretAgentPrivate::~NMSecretAgentPrivate()
    {
    }

}