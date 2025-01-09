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
#include <QObject>
#include "nm-secret-agent.h"

namespace Kiran
{
    class SecretsRequest
    {
    public:
        enum Type
        {
            GetSecrets,
            SaveSecrets,
            DeleteSecrets,
        };
        explicit SecretsRequest(Type _type)
            : type(_type), flags(NetworkManager::SecretAgent::None), saveSecretsWithoutReply(false)
        {
        }
        inline bool operator==(const QString &other) const
        {
            return callId == other;
        }
        Type type;
        QString callId;
        NMVariantMapMap connection;
        QDBusObjectPath connection_path;
        QString setting_name;
        QStringList hints;
        NetworkManager::SecretAgent::GetSecretsFlags flags;
        /**
         * When a user connection is called on GetSecrets,
         * the secret agent is supposed to save the secrets
         * typed by user, when true proccessSaveSecrets
         * should skip the DBus reply.
         */
        bool saveSecretsWithoutReply;
        QDBusMessage message;
        QString ssid;
    };

    class NMSecretAgentPrivate : public QObject
    {
        Q_OBJECT
        Q_DECLARE_PUBLIC(NMSecretAgent)
    public:
        explicit NMSecretAgentPrivate(NMSecretAgent *ptr = nullptr);
        ~NMSecretAgentPrivate();

    private:
        void processNext();
        /**
         * @brief processGetSecrets requests
         * @param request the request we are processing
         * @param ignoreWallet true if the code should avoid Wallet
         * normally if it failed to open
         * @return true if the item was processed
         */
        bool processGetSecrets(SecretsRequest &request);
        bool processSaveSecrets(SecretsRequest &request) const;
        bool processDeleteSecrets(SecretsRequest &request) const;

        /**
         * @brief hasSecrets verifies if the desired connection has secrets to store
         * @param connection map with or without secrets
         * @return true if the connection has secrets, false otherwise
         */
        bool hasSecrets(const NMVariantMapMap &connection) const;
        void sendSecrets(const NMVariantMapMap &secrets, const QDBusMessage &message) const;
        bool needConnectNetwork(const NMVariantMapMap &connectionMap) const;

    private:
        NMSecretAgent *q_ptr;
        QString m_requestedPasswordSsid;
        QList<SecretsRequest> m_calls;
    };
}