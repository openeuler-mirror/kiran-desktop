#include <QApplication>
#include <QLoggingCategory>
#include <QDebug>
#include "network-window.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    auto loggingCategory = QLoggingCategory::defaultCategory();
    loggingCategory->setEnabled(QtDebugMsg, true);

    NetworkWindow window;
    window.show();

    return app.exec();
}