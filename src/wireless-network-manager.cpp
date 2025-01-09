#include "wireless-network-manager.h"
#include "wireless-network-manager-private.h"
#include "def.h"
#include "logging-category.h"

#include <NetworkManagerQt/Utils>
#include <NetworkManagerQt/Ipv4Setting>
#include <NetworkManagerQt/Manager>
#include <QDBusPendingCallWatcher>

namespace Kiran
{
    using namespace NetworkManager;
    WirelessNetworkManager::WirelessNetworkManager(
        const WirelessDevice::Ptr &device,
        QObject *parent)
        : QObject(parent),
          d_ptr(new WirelessNetworkManagerPrivate(this, device, this))
    {
        d_ptr->init();
    }

    WirelessNetworkManager::WirelessNetworkManager(
        const QString &devicePath, QObject *parent)
        : WirelessNetworkManager(
              WirelessDevice::Ptr(new WirelessDevice(devicePath)),
              parent)
    {
    }

    WirelessNetworkManager::~WirelessNetworkManager()
    {
    }

    NetworkManager::Device::State WirelessNetworkManager::state() const
    {
        return d_ptr->m_device->state();
    }

    QString WirelessNetworkManager::activatedNetowrk() const
    {
        auto ap = d_ptr->m_device->activeAccessPoint();
        if (ap)
        {
            return ap->ssid();
        }
        return QString();
    }

    QString WirelessNetworkManager::uni() const
    {
        return d_ptr->m_device->uni();
    }

    QString WirelessNetworkManager::interfaceName() const
    {
        return d_ptr->m_device->interfaceName();
    }

    void WirelessNetworkManager::requestScan()
    {
        d_ptr->m_device->requestScan();
    }

    WirelessNetworkInfoList WirelessNetworkManager::getNetworkInfoList()
    {
        auto list = d_ptr->m_networkInfoMap.values();
        auto networkSortFunc = [](const WirelessNetworkInfo &a,
                                  const WirelessNetworkInfo &b) -> bool
        {
            return a.signalStreagth > b.signalStreagth;
        };
        std::sort(list.begin(), list.end(), networkSortFunc);
        return list;
    }

    WirelessNetworkInfo WirelessNetworkManager::getNetworkInfo(const QString &ssid)
    {
        WirelessNetworkInfo info;
        if (d_ptr->m_networkInfoMap.contains(ssid))
        {
            return d_ptr->m_networkInfoMap[ssid];
        }
        return info;
    }

    bool WirelessNetworkManager::checkNetworkCanDirectConn(const QString &ssid)
    {
        RETURN_VAL_IF_FALSE_WITH_WARNNING(d_ptr->m_networkInfoMap.contains(ssid),
                                          false,
                                          QString("network %1 not exists").arg(ssid));

        auto networkInfo = d_ptr->m_networkInfoMap[ssid];

        // 存在可直接使用的wifi连接配置
        auto connectioList = d_ptr->getWirelessNetworkConnection(ssid);
        return !connectioList.isEmpty();
    }

    WifiSecurityType WirelessNetworkManager::networkBestSecurityType(const QString &ssid)
    {
        RETURN_VAL_IF_FALSE_WITH_WARNNING(d_ptr->m_networkInfoMap.contains(ssid), SECURITY_TYPE_UNKNOWN,
                                          QString("network %1 not exists").arg(ssid));

        return d_ptr->getBestNetworkSecurity(ssid);
    }

    WifiSecurityTypeList WirelessNetworkManager::networkSupportSecurityTypeList(const QString &ssid)
    {
        RETURN_VAL_IF_FALSE_WITH_WARNNING(d_ptr->m_networkInfoMap.contains(ssid), {SECURITY_TYPE_UNKNOWN},
                                          QString("network %1 not exists").arg(ssid));

        return d_ptr->getSupportedNetworkSecuritys(ssid);
    }

    void WirelessNetworkManager::removeNetworkConnection(const QString &ssid)
    {
        Connection::Ptr res;
        auto connectionList = d_ptr->getWirelessNetworkConnection(ssid);
        if (connectionList.isEmpty())
        {
            KLOG_WARNING(qLcNetwork) << ssid << "no connection configuration available";
            return;
        }

        for (auto conn : connectionList)
        {
            KLOG_INFO(qLcNetwork) << "remove" << ssid << "connection" << conn->path();
            conn->remove();
        }
    }

    void WirelessNetworkManager::deactivateConnection()
    {
        ActiveConnection::Ptr activeConnection = NetworkManager::findActiveConnection(d_ptr->m_device->uni());
        if (activeConnection.isNull())
        {
            KLOG_WARNING(qLcNetwork) << interfaceName() << "deactivate connecion failed, no active connection";
            return;
        }

        KLOG_INFO(qLcNetwork) << "deactivate" << interfaceName()
                              << activeConnection->id()
                              << activeConnection->path();
        NetworkManager::deactivateConnection(activeConnection->path());
    }

    void WirelessNetworkManager::activateNetowrk(const QString &ssid)
    {
        RETURN_IF_FALSE_WITH_WARNNING(d_ptr->m_networkInfoMap.contains(ssid),
                                      QString("network %1 not exists").arg(ssid));

        auto networkInfo = d_ptr->m_networkInfoMap[ssid];
        auto connectionList = d_ptr->getWirelessNetworkConnection(ssid);

        if (connectionList.isEmpty())
            return;

        auto connection = connectionList.first();
        KLOG_INFO(qLcNetwork).nospace() << "activate " << d_ptr->m_device->interfaceName()
                                        << " network(" << ssid << ")"
                                        << " connection(" << connection->path() << ")";

        auto pendingReply = NetworkManager::activateConnection(connection->path(),
                                                               d_ptr->m_device->uni(),
                                                               networkInfo.referencePointPath);

        auto pendingCallWatcher = new QDBusPendingCallWatcher(pendingReply, this);
        pendingCallWatcher->setProperty(DBUS_WATCHER_PROPERTY_SSID, ssid);
        connect(pendingCallWatcher, &QDBusPendingCallWatcher::finished,
                d_ptr, &WirelessNetworkManagerPrivate::onActivateConnectionFinished);
        return;
    }

    void WirelessNetworkManager::addAndActivateHiddenNetwork(const QString &ssid,
                                                             const QString &password,
                                                             WifiSecurityType securityType)
    {
        KLOG_INFO(qLcNetwork).nospace() << "activate " << d_ptr->m_device->interfaceName()
                                        << " network(" << ssid << ")";

        auto connectionSettings = d_ptr->createConnectionSettings(ssid, securityType, password, true);
        if (connectionSettings.isNull())
        {
            KLOG_WARNING(qLcNetwork) << "create connection settings for" << ssid
                                     << "failed, unsupported security type";
            return;
        }

        auto pendingReply = addAndActivateConnection(connectionSettings->toMap(),
                                                     d_ptr->m_device->uni(),
                                                     "");

        auto pendingCallWatcher = new QDBusPendingCallWatcher(pendingReply, this);
        pendingCallWatcher->setProperty(DBUS_WATCHER_PROPERTY_SSID, ssid);
        connect(pendingCallWatcher, &QDBusPendingCallWatcher::finished,
                d_ptr, &WirelessNetworkManagerPrivate::onActivateConnectionFinished);
    }

    void WirelessNetworkManager::addAndActivateNetwork(const QString &ssid, const QString &password)
    {
        ConnectionSettings::Ptr connectionSettings;
        static QList<WifiSecurityType> recommendSecurityList = {
            SECURITY_TYPE_WPA3_PERSON,
            SECURITY_TYPE_WPA_AND_WPA2_PERSON,
            SECURITY_TYPE_NONE,
        };

        RETURN_IF_FALSE_WITH_WARNNING(d_ptr->m_networkInfoMap.contains(ssid),
                                      QString("network %1 not exists").arg(ssid));

        auto networkInfo = d_ptr->m_networkInfoMap[ssid];
        auto supportedSecurityTypes = d_ptr->getSupportedNetworkSecuritys(ssid);
        for (auto recommendSecurity : recommendSecurityList)
        {
            if (supportedSecurityTypes.contains(recommendSecurity))
            {
                connectionSettings = d_ptr->createConnectionSettings(ssid, recommendSecurity, password);
            }
        }

        if (connectionSettings.isNull())
        {
            KLOG_WARNING(qLcNetwork) << "create connection settings for" << ssid << "failed,"
                                     << "unsupported security type";
            return;
        }

        auto pendingReply = addAndActivateConnection(connectionSettings->toMap(),
                                                     d_ptr->m_device->uni(),
                                                     networkInfo.referencePointPath);
        auto pendingCallWatcher = new QDBusPendingCallWatcher(pendingReply, this);
        pendingCallWatcher->setProperty(DBUS_WATCHER_PROPERTY_SSID, ssid);
        connect(pendingCallWatcher, &QDBusPendingCallWatcher::finished,
                d_ptr, &WirelessNetworkManagerPrivate::onActivateConnectionFinished);
        return;
    }

    WirelessNetworkManagerPrivate::WirelessNetworkManagerPrivate(
        WirelessNetworkManager *ptr,
        const WirelessDevice::Ptr &wirelessDevice,
        QObject *parent)
        : QObject(parent),
          q_ptr(ptr),
          m_device(wirelessDevice)
    {
    }

    void WirelessNetworkManagerPrivate::init()
    {
        m_device->requestScan();
        loadNetworkInfoList();
#ifdef WIRELESS_NETWORK_SCAN_FINISHED_SIGNAL
        connect(m_device.data(), &WirelessDevice::lastScanChanged,
                q_ptr, &WirelessNetworkManager::scanFinished);
#endif
        connect(m_device.data(), &WirelessDevice::networkAppeared,
                this, &WirelessNetworkManagerPrivate::onNetworkAppeared);
        connect(m_device.data(), &WirelessDevice::networkDisappeared,
                this, &WirelessNetworkManagerPrivate::onNetworkDisappeared);
        connect(m_device.data(), &WirelessDevice::stateChanged,
                this, &WirelessNetworkManagerPrivate::onStateChanged);
        connect(m_device.data(), &WirelessDevice::activeAccessPointChanged,
                this, &WirelessNetworkManagerPrivate::onActiveAccessPointChanged);
        connect(m_device.data(), &Device::activeConnectionChanged,
                this, &WirelessNetworkManagerPrivate::onDeviceActiveConnectionChanged);
    }

    void WirelessNetworkManagerPrivate::loadNetworkInfoList()
    {
        m_networkInfoMap.clear();
        for (auto network : m_device->networks())
        {
            addNetwork(network->ssid());
        }

        KLOG_DEBUG(qLcNetwork) << "loaded network list" << m_networkInfoMap.values();
    }

    void WirelessNetworkManagerPrivate::onNetworkAppeared(const QString &ssid)
    {
        if (!addNetwork(ssid))
        {
            KLOG_WARNING(qLcNetwork) << "wireless network" << ssid << "appeared, but failed to be added";
        }
    }

    void WirelessNetworkManagerPrivate::onNetworkDisappeared(const QString &ssid)
    {
        if (!removeNetwork(ssid))
        {
            KLOG_WARNING(qLcNetwork) << "wireless network" << ssid << "disappeared, but failed to be remove";
        }
    }

    void WirelessNetworkManagerPrivate::onStateChanged(Device::State newstate, Device::State oldstate, Device::StateChangeReason reason)
    {
        emit q_ptr->stateChanged(newstate);
    }

    void WirelessNetworkManagerPrivate::onActiveAccessPointChanged(const QString &ap)
    {
    }

    void WirelessNetworkManagerPrivate::onActivateConnectionFinished(QDBusPendingCallWatcher *watcher)
    {
        auto reply = watcher->reply();
        auto ssid = watcher->property(DBUS_WATCHER_PROPERTY_SSID).toString();
        KLOG_DEBUG(qLcNetwork) << ssid << "activate connection pending call finished" << reply;
    }

    /**
     * 设备ActiveConnection改变处理方法：
     * 通过激活连接，拿到连接信息，再拿到连接配置
     * 通过连接配置中填写的ssid信息，拿到所激活的无线网络
     */
    void WirelessNetworkManagerPrivate::onDeviceActiveConnectionChanged()
    {
        QString activeNetworkSsid = "";
        auto activeConn = m_device->activeConnection();

        if (!activeConn.isNull())
        {
            KLOG_WARNING(qLcNetwork) << q_ptr->interfaceName()
                                     << "active network is empty";
        }
        else
        {
            auto connection = activeConn->connection();
            auto connectionSettings = connection->settings();
            auto wirelessSetting = connectionSettings->setting(Setting::Wireless).dynamicCast<WirelessSetting>();
            activeNetworkSsid = wirelessSetting->ssid();
            KLOG_DEBUG(qLcNetwork) << q_ptr->interfaceName()
                                   << "active network"  << activeNetworkSsid 
                                   << ", with connection settings" << wirelessSetting->name();
        }

        emit q_func() -> activeNetworkChanged(activeNetworkSsid);
    }

    bool WirelessNetworkManagerPrivate::addNetwork(const QString &ssid, bool isInit)
    {
        auto network = m_device->findNetwork(ssid);

        RETURN_VAL_IF_FALSE_WITH_WARNNING(!network.isNull(), false,
                                          QString("can't find %1").arg(ssid));
        RETURN_VAL_IF_FALSE_WITH_WARNNING(!m_networkInfoMap.contains(ssid), false,
                                          QString("network %1 already exists").arg(ssid));

        m_networkInfoMap.insert(ssid, WirelessNetworkInfo(network->ssid(),
                                                          network->referenceAccessPoint()->uni(),
                                                          network->signalStrength()));
        RETURN_VAL_IF_TRUE(isInit, true);

        emit q_ptr->networkAppeared(ssid);
        return true;
    }

    bool WirelessNetworkManagerPrivate::removeNetwork(const QString &ssid)
    {
        RETURN_VAL_IF_FALSE_WITH_WARNNING(m_networkInfoMap.contains(ssid), false,
                                          QString("network %1 not exists").arg(ssid));

        m_networkInfoMap.remove(ssid);
        emit q_ptr->networkDisappeared(ssid);
        return true;
    }

    WifiSecurityType WirelessNetworkManagerPrivate::convertNMSecurityType(WirelessSecurityType securityType)
    {
        switch (securityType)
        {
        case NoneSecurity:
            return SECURITY_TYPE_NONE;
        case SAE:
            return SECURITY_TYPE_WPA3_PERSON;
        case WpaPsk:
        case Wpa2Psk:
            return SECURITY_TYPE_WPA_AND_WPA2_PERSON;
        case Wpa2Eap:
        case WpaEap:
            return SECURITY_TYPE_WPA_AND_WPA2_ENTERPRISE;
        default:
            break;
        }
        return SECURITY_TYPE_UNKNOWN;
    }

    QList<WirelessSecurityType> WirelessNetworkManagerPrivate::getAPSecuritySupport(const QString &ap)
    {
        QList<WirelessSecurityType> securityList;
        auto accessPointer = m_device->findAccessPoint(ap);
        if (!accessPointer)
        {
            return securityList;
        }

        auto deviceCapabilities = m_device->wirelessCapabilities();
        auto apCapabilities = accessPointer->capabilities();
        auto apRsn = accessPointer->rsnFlags();
        auto apWpa = accessPointer->wpaFlags();

        const QList<NetworkManager::WirelessSecurityType> types = {NetworkManager::SAE,
                                                                   NetworkManager::Wpa2Psk,
                                                                   NetworkManager::WpaPsk,
                                                                   NetworkManager::Wpa2Eap,
                                                                   NetworkManager::WpaEap,
                                                                   NetworkManager::NoneSecurity};

        for (NetworkManager::WirelessSecurityType type : types)
        {
            if (NetworkManager::securityIsValid(type, deviceCapabilities, true, false, apCapabilities, apWpa, apRsn))
            {
                securityList << type;
            }
        }

        return securityList;
    }

    WifiSecurityType WirelessNetworkManagerPrivate::getBestNetworkSecurity(const QString &ssid)
    {
        RETURN_VAL_IF_FALSE_WITH_WARNNING(m_networkInfoMap.contains(ssid),
                                          SECURITY_TYPE_UNKNOWN,
                                          QString("network %1 not exists").arg(ssid));

        auto networkInfo = m_networkInfoMap[ssid];

        // 若AP不存在(隐藏网络)直接走WpaPSK
        if (networkInfo.referencePointPath.isEmpty())
        {
            return SECURITY_TYPE_WPA_AND_WPA2_PERSON;
        }

        auto securityTypeList = getAPSecuritySupport(networkInfo.referencePointPath);
        if (securityTypeList.isEmpty())
        {
            return SECURITY_TYPE_UNKNOWN;
        }

        return convertNMSecurityType(securityTypeList.first());
    }

    WifiSecurityTypeList WirelessNetworkManagerPrivate::getSupportedNetworkSecuritys(const QString &ssid)
    {
        WifiSecurityTypeList securityList;

        if (!m_networkInfoMap.contains(ssid))
        {
            return securityList;
        }

        auto networkInfo = m_networkInfoMap[ssid];
        if (networkInfo.referencePointPath.isEmpty())
        {
            return WifiSecurityTypeList{
                SECURITY_TYPE_NONE,
                SECURITY_TYPE_WPA_AND_WPA2_PERSON,
                SECURITY_TYPE_WPA_AND_WPA2_ENTERPRISE,
                SECURITY_TYPE_WPA3_PERSON,
            };
        }

        for (auto securityType : getAPSecuritySupport(networkInfo.referencePointPath))
        {
            auto internalSecurityType = convertNMSecurityType(securityType);
            if (!securityList.contains(internalSecurityType))
            {
                securityList << internalSecurityType;
            }
        }

        return securityList;
    }

    Connection::List WirelessNetworkManagerPrivate::getWirelessNetworkConnection(const QString &ssid)
    {
        Connection::List res;
        auto connectionList = m_device->availableConnections();
        for (auto connection : connectionList)
        {
            auto connectionSettings = connection->settings();

            // 检查配置类型
            if (connectionSettings->connectionType() != ConnectionSettings::Wireless)
            {
                continue;
            }

            // 检查配置中是否指定网卡名
            if (!connectionSettings->interfaceName().isEmpty() &&
                (connectionSettings->interfaceName() != m_device->interfaceName()))
            {
                continue;
            }

            auto settingPtr = connection->settings()->setting(Setting::SettingType::Wireless);
            if (settingPtr.isNull())
            {
                continue;
            }

            // 检查配置中是否绑定了设备MAC地址
            QString deviceHardwareAddress = m_device->permanentHardwareAddress();
            if (deviceHardwareAddress.isEmpty())
            {
                deviceHardwareAddress = m_device->hardwareAddress();
            }

            auto wirelessSetting = settingPtr.dynamicCast<WirelessSetting>();
            if (!wirelessSetting->macAddress().isEmpty() &&
                (wirelessSetting->macAddress() != deviceHardwareAddress))
            {
                continue;
            }

            // 检查配置ssid匹配
            if (QString(wirelessSetting->ssid()) == ssid)
            {
                res << connection;
            }
        }

        return res;
    }

    ConnectionSettings::Ptr WirelessNetworkManagerPrivate::createConnectionSettings(const QString &ssid,
                                                                                    WifiSecurityType securityType,
                                                                                    const QString &password,
                                                                                    bool hidden)
    {
        auto settingsPtr = ConnectionSettings::Ptr(new ConnectionSettings(ConnectionSettings::Wireless));
        settingsPtr->setId(ssid);
        settingsPtr->setUuid(ConnectionSettings::createNewUuid());

        auto wirelessSetting = settingsPtr->setting(Setting::Wireless).dynamicCast<WirelessSetting>();
        wirelessSetting->setInitialized(true);
        wirelessSetting->setSsid(ssid.toUtf8());
        wirelessSetting->setHidden(hidden);

        auto wirelessSecurity = settingsPtr->setting(Setting::WirelessSecurity).dynamicCast<WirelessSecuritySetting>();
        wirelessSecurity->setInitialized(true);
        wirelessSetting->setSecurity(QStringLiteral("802-11-wireless-security"));

        auto ipv4Setting = settingsPtr->setting(Setting::Ipv4).dynamicCast<Ipv4Setting>();
        ipv4Setting->setMethod(Ipv4Setting::Automatic);

        switch (securityType)
        {
        case SECURITY_TYPE_NONE:
        {
            wirelessSecurity->setKeyMgmt(WirelessSecuritySetting::KeyMgmt::WpaNone);
            break;
        }
        case SECURITY_TYPE_WPA_AND_WPA2_PERSON:
        {
            wirelessSecurity->setKeyMgmt(WirelessSecuritySetting::KeyMgmt::WpaPsk);
            wirelessSecurity->setPsk(password);
            wirelessSecurity->setPskFlags(Setting::SecretFlagType::None);
            break;
        }
        case SECURITY_TYPE_WPA3_PERSON:
        {
            wirelessSecurity->setKeyMgmt(WirelessSecuritySetting::KeyMgmt::SAE);
            wirelessSecurity->setPsk(password);
            wirelessSecurity->setPskFlags(Setting::SecretFlagType::None);
            break;
        }
        default:
            KLOG_WARNING(qLcNetwork) << "create connection settings for" << ssid
                                     << "unsupported security type";
            return ConnectionSettings::Ptr();
        }

        // default: Save password for all users
        wirelessSecurity->setInitialized(true);
        return settingsPtr;
    }
}