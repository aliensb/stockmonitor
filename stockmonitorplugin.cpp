#include "stockmonitorplugin.h"

StockMonitorPlugin::StockMonitorPlugin(QObject *parent)
    : QObject(parent)
    , m_refreshTimer(new QTimer(this))
{
    list=new QStringList();
    manager = new QNetworkAccessManager;
    request=new QNetworkRequest;
    currentStockInfo= new stockInfo;
    
}

const QString StockMonitorPlugin::pluginName() const
{
    return QStringLiteral("home_monitor");
}

void StockMonitorPlugin::init(PluginProxyInterface *proxyInter)
{
    m_proxyInter = proxyInter;
    //添加到任务栏的容器
    infoWidget = new InformationWidget;
   
   //鼠标指上面显示打对话框
    tipsLabel = new QLabel; // new
    tipsLabel->setStyleSheet("color:white; padding:0px 3px;");

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
    currentStockInfo->code=list->at(0);
    // m_pluginWidget->regist(this);
    
    refresh();
    connect(m_refreshTimer, &QTimer::timeout, this, &StockMonitorPlugin::refresh);
    m_refreshTimer->start(500);
    m_proxyInter->itemAdded(this, pluginName());

    // 如果插件没有被禁用则在初始化插件时才添加主控件到面板上
    if (!pluginIsDisable()) {
        m_proxyInter->itemAdded(this, pluginName());
    }
    
}

QWidget *StockMonitorPlugin::itemWidget(const QString &itemKey)
{
    Q_UNUSED(itemKey);

    // 这里暂时返回空指针，这意味着插件会被 dde-dock 加载
    // 但是不会有任何东西被添加到 dde-dock 上
    return infoWidget;
}

bool StockMonitorPlugin::pluginIsAllowDisable()
{
    // 告诉 dde-dock 本插件允许禁用
    return true;
}

bool StockMonitorPlugin::pluginIsDisable()
{
    // 第二个参数 “disabled” 表示存储这个值的键（所有配置都是以键值对的方式存储的）
    // 第三个参数表示默认值，即默认不禁用
    return m_proxyInter->getValue(this, "disabled", false).toBool();
}

void StockMonitorPlugin::pluginStateSwitched()
{
    // 获取当前禁用状态的反值作为新的状态值
    const bool disabledNew = !pluginIsDisable();
    // 存储新的状态值
    m_proxyInter->saveValue(this, "disabled", disabledNew);

    // 根据新的禁用状态值处理主控件的加载和卸载
    //qDebug()<<disabledNew;
    if (disabledNew) {
        m_proxyInter->itemRemoved(this, pluginName());
        disconnect(m_refreshTimer, 0, this, 0);
    } else {
        m_proxyInter->itemAdded(this, pluginName());
        connect(m_refreshTimer, &QTimer::timeout, this, &StockMonitorPlugin::refresh);
    }
}

const QString StockMonitorPlugin::pluginDisplayName() const
{
    return QString("stock Monitor");
}

QWidget *StockMonitorPlugin::itemTipsWidget(const QString &itemKey)
{
    Q_UNUSED(itemKey);

    // 设置/刷新 tips 中的信息
    auto res= QString("%1：%2\n开盘价：%3\n昨日收盘：%4\n今日最高: %5\n今日最低: %6\n").arg(currentStockInfo->name)
    .arg(currentStockInfo->code)
    .arg(currentStockInfo->open)
    .arg(currentStockInfo->old)
    .arg(currentStockInfo->todayHigh)
    .arg(currentStockInfo->todayLow);
    tipsLabel->setText(res);
    return tipsLabel;
}

const QString StockMonitorPlugin::itemContextMenu(const QString &itemKey)
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

void StockMonitorPlugin::invokedMenuItem(const QString &itemKey, const QString &menuId, const bool checked)
{
    Q_UNUSED(itemKey);

    // 根据上面接口设置的 id 执行不同的操作
    if (menuId == "setting") {
        qDebug()<<"setting";
         QMessageBox messageBox(QMessageBox::NoIcon,
                           "退出", "你确定要退出吗?",
                           QMessageBox::Yes | QMessageBox::No, NULL); ;
        int result=messageBox.exec();
        switch (result)
        {
        case QMessageBox::Yes:
            qDebug()<<"Yes";
            break;
        case QMessageBox::No:
            qDebug()<<"NO";
            break;
        default:
            break;
        }
    } 
}

const QString StockMonitorPlugin::itemCommand(const QString &itemKey){
        qDebug()<<"click";
        int index=list->indexOf(currentStockInfo->code);
        if(index+1==list->size()){
            index=0;
        }else{
            index++;
        }
        qDebug()<<QString("next index  is %1").arg(index);
        currentStockInfo->code=list->at(index);
        qDebug()<<QString("cureent currentCode  is %1").arg(currentStockInfo->code);
        refresh();
        return QString();
}

void StockMonitorPlugin::getStockInfo(QString url,const char *charset){
    //qDebug()<<QString("请求路径为 %1").arg(url);
   // QNetworkAccessManager *manager = new QNetworkAccessManager();
   // QNetworkRequest request;
    request->setUrl(QUrl(url));
    // try{

    QNetworkReply *pReply = manager->get(*request);
        // 开启一个局部的事件循环，等待页面响应结束
    QEventLoop eventLoop;
    QObject::connect(manager, &QNetworkAccessManager::finished, &eventLoop, &QEventLoop::quit);
    eventLoop.exec();
        // 获取网页Body中的内容
    QByteArray bytes = pReply->readAll();
    QTextCodec *tc = QTextCodec::codecForName(charset);

    QString str = tc->toUnicode(bytes);
    QStringList list=str.split(",");
    //qDebug()<<QString("数据长度为 %1").arg(list.size());
     if(list.size()<4){
         return ;
     }
     currentStockInfo->name=list.at(0).split("\"").at(1);
     currentStockInfo->open=list.at(1).toDouble();
     currentStockInfo->now=list.at(3).toDouble();
     currentStockInfo->old=list.at(2).toDouble();
     currentStockInfo->percent=(currentStockInfo->now-currentStockInfo->old)/currentStockInfo->old;
     currentStockInfo->todayHigh=list.at(4).toDouble();
     currentStockInfo->todayLow=list.at(5).toDouble();
     

    // }catch(QString exception){
    //  currentStockInfo->name="网络错误，请稍后再试";
    //  currentStockInfo->open=0;
    //  currentStockInfo->now=0;
    //  currentStockInfo->old=0;
    //  currentStockInfo->percent=0;
    //     QMessageBox::about(nullptr,"Error",exception);
    // }
    // qDebug()<<QString("%1%2%3%4%5")
    // .arg(currentStockInfo->name)
    // .arg(currentStockInfo->open)
    // .arg(currentStockInfo->now)
    // .arg(currentStockInfo->old)
    // .arg(currentStockInfo->percent);
    return ;
}




void StockMonitorPlugin::refresh(){
     // 更新内容`
    //qDebug()<<QString("请求开始时间 %1").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm::ss.zzz"));
    getStockInfo("http://hq.sinajs.cn/list="+currentStockInfo->code,"GB18030");
    //qDebug()<<QString("请求结束时间 %1").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm::ss.zzz"));
    // parseData(res);
    QString flag="";
    QString style="";
    if(currentStockInfo->percent>=0){
        style="QLabel {"
                               "color: red;"
                               "padding: auto 10px;"
                               "}";
        flag="↑";
    }else{
        style="QLabel {"
                               "color: green;"
                               "padding: auto 10px;"
                               "}";
         flag="↓";
    }
    auto percent=QString::number(currentStockInfo->percent*100, 'f', 2);
    auto text=QString("%1: %2 %3 %4%").arg(currentStockInfo->name).arg(currentStockInfo->now).arg(flag).arg(percent);
    
    infoWidget->setTextAndStyle(text,style);
    // if(currentStockInfo->percent>0.05||currentStockInfo->percent<-0.05){
    //     //qDebug()<<"准备警告";
    //    if(!isShow){
    //         QMessageBox::information(NULL, "股价警告", text, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    //         isShow=true;
    //    }
    // }else{
    //     isShow=false;
    // }
}