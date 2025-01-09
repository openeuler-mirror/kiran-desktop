#include <QApplication>
#include <QLoggingCategory>
#include <QDebug>
#include <NetworkManagerQt/Manager>
#include "network-window.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    auto loggingCategory = QLoggingCategory::defaultCategory();
    loggingCategory->setEnabled(QtDebugMsg, true);
    
    bool hasWirelessDevice = false;
    const auto deviceList = NetworkManager::networkInterfaces();
    for (auto device : deviceList)
    {
        if ( device->type() == NetworkManager::Device::Wifi)
        {
            hasWirelessDevice = true;
            break;
        }
    }

    if( !hasWirelessDevice )
    {
        qFatal("No wireless card device detected!");
        return EXIT_FAILURE;
    }

    NetworkWindow window;
    window.show();

    return app.exec();
}