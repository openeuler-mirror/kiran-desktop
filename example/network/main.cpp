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