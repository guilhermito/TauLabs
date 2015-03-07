#ifndef ICONFIGURABLEPLUGIN_H
#define ICONFIGURABLEPLUGIN_H

#include <QObject>
#include <QSettings>
#include <extensionsystem/iplugin.h>
#include <slimcoreplugin/uavconfiginfo.h>

#include <slimcoreplugin/core_global.h>

namespace Core {

class CORE_EXPORT IConfigurablePlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
public:
    // IConfigurablePlugin(QObject *parent = 0){}
    virtual ~IConfigurablePlugin() {}
    virtual void readConfig( QSettings* qSettings, UAVConfigInfo *configInfo) = 0;
    virtual void saveConfig(QSettings* qSettings, Core::UAVConfigInfo *configInfo) = 0;
};

} // namespace Core

#endif // ICONFIGURABLEPLUGIN_H
