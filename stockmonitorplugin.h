#ifndef HOMEMONITORPLUGIN_H
#define HOMEMONITORPLUGIN_H

#include <dde-dock/pluginsiteminterface.h>
#include <QFile>
#include <QObject>
#include <QLabel>
#include <QTimer>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QMessageBox>
#include "informationwidget.h"
#include "stockinfo.h"
#include <map>

class StockMonitorPlugin : public QObject, PluginsItemInterface
{
    Q_OBJECT
    // 声明实现了的接口
    Q_INTERFACES(PluginsItemInterface)
    // 插件元数据
    Q_PLUGIN_METADATA(IID "com.deepin.dock.PluginsItemInterface" FILE "stock_monitor.json")

public:
    explicit StockMonitorPlugin(QObject *parent = nullptr);

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
    const QString itemContextMenu(const QString &itemKey) override;
    void invokedMenuItem(const QString &itemKey, const QString &menuId, const bool checked) override;
    const QString itemCommand(const QString &itemKey) override;

   
private:
    void getStockInfo(QString url,const char *charset);
    void getMutiStockInfo();
    //void parseData(QString data);
    stockInfo *currentStockInfo;
    void refresh();
    InformationWidget *infoWidget;
    QLabel * tipsLabel;
    //股票代码集合
    QStringList *list;
    //股票代码集合,形式如下sz399001,sh000001
    QString codes;
    std::map<QString,stockInfo*> *stocks;
     //显示容器
    QWidget *itemTipsWidget(const QString &itemKey) override;
    // //当前显示的股票代码
    // QString currentCode;

    //定时器
    QTimer *m_refreshTimer;
    // 分区数据的来源
    //QStorageInfo *m_storageInfo;
    QNetworkAccessManager *manager;
    QNetworkRequest *request;
    bool isShow;


    
};

#endif // HOMEMONITORPLUGIN_H
