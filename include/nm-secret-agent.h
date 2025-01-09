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
#include <NetworkManagerQt/SecretAgent>
namespace Kiran
{
    class NMSecretAgentPrivate;
    class Q_DECL_EXPORT NMSecretAgent : public NetworkManager::SecretAgent
    {
        Q_OBJECT
        Q_DECLARE_PRIVATE(NMSecretAgent);

    public:
        explicit NMSecretAgent(QObject *parent = nullptr);
        ~NMSecretAgent() override;

    Q_SIGNALS:
        void requestPassword(const QString &devicePath, const QString &ssid, bool wait);

    public slots:
        void respondPasswdRequest(const QString &ssid, const QString &password, bool isCancel);

    private Q_SLOTS:
        NMVariantMapMap GetSecrets(const NMVariantMapMap &, const QDBusObjectPath &, const QString &, const QStringList &, uint) override;
        void SaveSecrets(const NMVariantMapMap &connection, const QDBusObjectPath &connection_path) override;
        void DeleteSecrets(const NMVariantMapMap &, const QDBusObjectPath &) override;
        void CancelGetSecrets(const QDBusObjectPath &, const QString &) override;

    private:
        NMSecretAgentPrivate *d_ptr;
    };
};