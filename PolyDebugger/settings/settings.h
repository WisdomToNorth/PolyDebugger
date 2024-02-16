#ifndef DEBUGGER_SETTINGS_H
#define DEBUGGER_SETTINGS_H

#include <QQuickItem>

#include "adaptor/geometrycanvasitem.h"

class NgSettings : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(AppAlgorithmCore appAlgorithmCore READ appAlgorithmCore WRITE setAppAlgorithmCore
                   NOTIFY appAlgorithmCoreChanged)

public:
    enum AppAlgorithmCore
    {
        kCavc,
        kNGPoly,
        kClipper
    };
    Q_ENUM(AppAlgorithmCore)

    static NgSettings &instance()
    {
        static NgSettings instance;
        return instance;
    }

signals:
    void appAlgorithmCoreChanged(AppAlgorithmCore appAlgorithmCore);

public:
    NgSettings(QQuickItem *parent = nullptr){};

    AppAlgorithmCore appAlgorithmCore() const
    {
        return g_machineType;
    }

    void setAppAlgorithmCore(AppAlgorithmCore appAlgorithmCore)
    {
        if (g_machineType != appAlgorithmCore)
        {
            g_machineType = appAlgorithmCore;
            emit appAlgorithmCoreChanged(appAlgorithmCore);
            std::cout << "cur algorithm core: " << static_cast<int>(g_machineType) << std::endl;
        }
    }

private:
    AppAlgorithmCore g_machineType = kNGPoly;
};

#endif // PLINECOMBINEALGORITHMVIEW_H
