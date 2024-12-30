#include <QApplication>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/WirelessDevice>
#include <QDebug>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include "wireless-network-manager.h"
#include <QLoggingCategory>

static bool isDebug = false;

void dumpWirelessDevice(NetworkManager::WirelessDevice::Ptr wirelessDevice)
{
    qInfo() << "wifi interfacename:" << wirelessDevice->interfaceName();
    qInfo() << "driver name:" << wirelessDevice->driver() << "driver version:" << wirelessDevice->driverVersion();
    qInfo() << "is active:" << wirelessDevice->isActive() << "is valid:" << wirelessDevice->isValid() << "state:" << wirelessDevice->state() << wirelessDevice->stateReason().reason();
    qInfo() << "design speed:" << wirelessDevice->designSpeed();
    qInfo() << "wireless capabilities:" << wirelessDevice->wirelessCapabilities();
    qInfo() << "last scan:" << wirelessDevice->lastScan().toString("yyyy-MM-dd hh:mm:ss");

    auto networkList = wirelessDevice->networks();
    for (auto network : networkList)
    {
        qInfo() << "\tnetwork " << network->ssid();
        qInfo() << "\t\tstrength:" << network->signalStrength();
        qInfo() << "\t\treference access point:" << network->referenceAccessPoint()->ssid() << network->referenceAccessPoint()->hardwareAddress();

        auto accessPoints = network->accessPoints();
        for (auto accessPoint : accessPoints)
        {
            qInfo() << "\t\taccessPoint " << accessPoint->ssid() << accessPoint->uni() << accessPoint->hardwareAddress();
            qInfo() << "\t\t\tfrequency:" << accessPoint->frequency();
            qInfo() << "\t\t\tcapabilities:" << accessPoint->capabilities();
            qInfo() << "\t\t\tmax bit rate:" << accessPoint->maxBitRate() << "Kb/s";
        }
    }
    qInfo() << "========================";
    auto connections = wirelessDevice->availableConnections();
    for (auto connection : connections)
    {
        qInfo() << "\tconnection " << connection->name() << connection->uuid() << connection->isValid();
    }
}

void processCommandLine()
{
    if (!qApp)
    {
        qWarning() << "process command line failed, qApp is null";
        return;
    }

    QCommandLineParser cmdParser;
    QCommandLineOption debugOption("d", "debug dump wireless device", "debug");

    cmdParser.addHelpOption();
    cmdParser.addOptions({debugOption});
    cmdParser.process(*qApp);

    if (cmdParser.isSet(debugOption))
    {
        isDebug = true;
    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    auto loggingCategory = QLoggingCategory::defaultCategory();
    loggingCategory->setEnabled(QtDebugMsg, true);

    processCommandLine();

    Kiran::WirelessNetworkManager *deviceManager = NULL;
    const auto deviceList = NetworkManager::networkInterfaces();
    for (auto device : deviceList)
    {
        auto deviceType = device->type();
        if (deviceType != NetworkManager::Device::Wifi)
        {
            continue;
        }

        auto wirelessDevice = device.objectCast<NetworkManager::WirelessDevice>();

        if (isDebug)
            dumpWirelessDevice(wirelessDevice);

        deviceManager = new Kiran::WirelessNetworkManager(wirelessDevice);
        QObject::connect(deviceManager, &Kiran::WirelessNetworkManager::scanFinished, []()
                         { qInfo() << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "scan finished"; });

        auto networkInfoList = deviceManager->getNetworkInfoList();
        for (auto networkInfo : networkInfoList)
        {
            qInfo() << networkInfo.ssid << networkInfo.referencePointPath << networkInfo.signalStreagth;
        }
    }

    QString ssid = "科技园1";
    qInfo() << "best secuirty type:" << deviceManager->networkBestSecurityType(ssid);
    qInfo() << "supported secuirty types:" << deviceManager->networkSupportSecurityTypeList(ssid);
    auto networkInfo = deviceManager->getNetworkInfo(ssid);
    if (deviceManager->checkNetworkCanDirectConn(ssid))
    {
        qInfo() << "activate network:" << ssid;
        deviceManager->activateNetowrk(ssid);
    }
    else
    {
        qInfo() << "activate network by passwd:" << ssid;
        deviceManager->addAndActivateNetwork(ssid, "12345678");
    }

    return app.exec();
}