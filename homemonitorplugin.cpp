#include "homemonitorplugin.h"

HomeMonitorPlugin::HomeMonitorPlugin(QObject *parent)
    : QObject(parent)
{
    list=new QStringList();
    
}

const QString HomeMonitorPlugin::pluginName() const
{
    return QStringLiteral("home_monitor");
}

void HomeMonitorPlugin::init(PluginProxyInterface *proxyInter)
{
    m_proxyInter = proxyInter;

    m_pluginWidget = new InformationWidget;
    m_pluginWidget->adjustSize();
   
    m_tipsWidget = new QLabel; // new
    //添加主控件到 dde-dock 面板上

    //从文件中读取codes;
    QFile file("/home/tom/stockCodes");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"Can't open the file!"<<endl;
    }
    while(!file.atEnd())
    {
        QByteArray line = file.readLine();
        QString str(line);
        list->push_back(str.trimmed());
    }
    currentCode=list->at(0);
    m_pluginWidget->regist(this);
    
    m_proxyInter->itemAdded(this, pluginName());

    // 如果插件没有被禁用则在初始化插件时才添加主控件到面板上
    if (!pluginIsDisable()) {
        m_proxyInter->itemAdded(this, pluginName());
    }
    
}

QWidget *HomeMonitorPlugin::itemWidget(const QString &itemKey)
{
    Q_UNUSED(itemKey);

    // 这里暂时返回空指针，这意味着插件会被 dde-dock 加载
    // 但是不会有任何东西被添加到 dde-dock 上
    return m_pluginWidget;
}

bool HomeMonitorPlugin::pluginIsAllowDisable()
{
    // 告诉 dde-dock 本插件允许禁用
    return true;
}

bool HomeMonitorPlugin::pluginIsDisable()
{
    // 第二个参数 “disabled” 表示存储这个值的键（所有配置都是以键值对的方式存储的）
    // 第三个参数表示默认值，即默认不禁用
    return m_proxyInter->getValue(this, "disabled", false).toBool();
}

void HomeMonitorPlugin::pluginStateSwitched()
{
    // 获取当前禁用状态的反值作为新的状态值
    const bool disabledNew = !pluginIsDisable();
    // 存储新的状态值
    m_proxyInter->saveValue(this, "disabled", disabledNew);

    // 根据新的禁用状态值处理主控件的加载和卸载
    if (disabledNew) {
        m_proxyInter->itemRemoved(this, pluginName());
    } else {
        m_proxyInter->itemAdded(this, pluginName());
    }
}

const QString HomeMonitorPlugin::pluginDisplayName() const
{
    return QString("stock Monitor");
}

QWidget *HomeMonitorPlugin::itemTipsWidget(const QString &itemKey)
{
    Q_UNUSED(itemKey);

    // 设置/刷新 tips 中的信息
    m_tipsWidget->setText("hover tips");

    return m_tipsWidget;
}

const QString HomeMonitorPlugin::itemContextMenu(const QString &itemKey)
{
    Q_UNUSED(itemKey);

    QList<QVariant> items;
    items.reserve(1);

    QMap<QString, QVariant> refresh;
    refresh["itemId"] = "setting";
    refresh["itemText"] = "setting";
    refresh["isActive"] = true;
    items.push_back(refresh);


    QMap<QString, QVariant> menu;
    menu["items"] = items;
    menu["checkableMenu"] = false;
    menu["singleCheck"] = false;

    // 返回 JSON 格式的菜单数据
    return QJsonDocument::fromVariant(menu).toJson();
}

void HomeMonitorPlugin::invokedMenuItem(const QString &itemKey, const QString &menuId, const bool checked)
{
    Q_UNUSED(itemKey);

    // 根据上面接口设置的 id 执行不同的操作
    if (menuId == "setting") {
        qDebug()<<"setting";
    } 
}

const QString HomeMonitorPlugin::itemCommand(const QString &itemKey){
        qDebug()<<"click";
        int index=list->indexOf(currentCode);
        if(index+1==list->size()){
            index=0;
        }else{
            index++;
        }
        qDebug()<<QString("next index  is %1").arg(index);
        currentCode=list->at(index);
        qDebug()<<QString("cureent currentCode  is %1").arg(currentCode);
        m_pluginWidget->refreshInfo();
        return QString();
}