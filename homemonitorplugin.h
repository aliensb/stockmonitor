#ifndef HOMEMONITORPLUGIN_H
#define HOMEMONITORPLUGIN_H

#include <dde-dock/pluginsiteminterface.h>
#include <QFile>
#include <QObject>
#include "informationwidget.h"
class InformationWidget;
class HomeMonitorPlugin : public QObject, PluginsItemInterface
{
    Q_OBJECT
    // 声明实现了的接口
    Q_INTERFACES(PluginsItemInterface)
    // 插件元数据
    Q_PLUGIN_METADATA(IID "com.deepin.dock.PluginsItemInterface" FILE "home_monitor.json")

public:
    explicit HomeMonitorPlugin(QObject *parent = nullptr);

    // 返回插件的名称，必须是唯一值，不可以和其它插件冲突
    const QString pluginName() const override;

    // 插件初始化函数
    void init(PluginProxyInterface *proxyInter) override;

    // 返回插件的 widget
    QWidget *itemWidget(const QString &itemKey) override;

    bool pluginIsAllowDisable() override;
    bool pluginIsDisable() override;
    void pluginStateSwitched() override;
    const QString pluginDisplayName() const override;
    QWidget *itemTipsWidget(const QString &itemKey) override;
    const QString itemContextMenu(const QString &itemKey) override;
    void invokedMenuItem(const QString &itemKey, const QString &menuId, const bool checked) override;
    const QString itemCommand(const QString &itemKey) override;
    QString currentCode;
private:
    InformationWidget *m_pluginWidget;
    QLabel *m_tipsWidget; //支持 hover tip
    QStringList *list;

    
};

#endif // HOMEMONITORPLUGIN_H
