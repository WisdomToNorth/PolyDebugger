#ifndef DEBUGGER_SETTINGS_H
#define DEBUGGER_SETTINGS_H

#include <QQuickItem>

#include "adaptor/geometrycanvasitem.h"

namespace debugger
{
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

    NgSettings(QQuickItem *parent = nullptr){};
    static AppAlgorithmCore getCurAlgorithmCore()
    {
        return g_machineType;
    }

signals:
    void appAlgorithmCoreChanged(AppAlgorithmCore appAlgorithmCore);

public:
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
    static AppAlgorithmCore g_machineType;
};
} // namespace debugger

#endif // PLINECOMBINEALGORITHMVIEW_H
