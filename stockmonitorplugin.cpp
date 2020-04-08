#include "stockmonitorplugin.h"

StockMonitorPlugin::StockMonitorPlugin(QObject *parent)
    : QObject(parent)
    , m_refreshTimer(new QTimer(this))
    , rollingTimmer(new QTimer(this))
{
    list=new QStringList();
    manager = new QNetworkAccessManager;
    request=new QNetworkRequest;
    currentStockInfo= new stockInfo;
    stocks=new std::map<QString,stockInfo*>;
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
    QFile file(QStandardPaths::writableLocation(QStandardPaths::HomeLocation) +"/stockCodes");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"Can't open the file!"<<endl;
    }
    while(!file.atEnd())
    {
        QByteArray line = file.readLine();
        QString str(line);
        list->push_back(str.trimmed());
        stockInfo *st=new stockInfo();
        st->code=str.trimmed();
        stocks->insert(std::pair<QString,stockInfo*>(str.trimmed(), st));
    }
    codes=list->join(",");

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
    // auto res= QString("%1：%2\n开盘价：%3\n昨日收盘：%4\n今日最高: %5\n今日最低: %6\n").arg(currentStockInfo->name)
    // .arg(currentStockInfo->code)
    // .arg(currentStockInfo->open)
    // .arg(currentStockInfo->old)
    // .arg(currentStockInfo->todayHigh)
    // .arg(currentStockInfo->todayLow);
    // tipsLabel->setText(res);
    //getMutiStockInfo();
    auto st=stocks->at(currentStockInfo->code);
    QString res=
        QString("%1\t%2\n开盘价格\t%3\n昨日收盘\t%4\n今日最高\t%5\n今日最低\t%6\n")
        .arg(st->name)
        .arg(st->code)
        .arg(st->open)
        .arg(st->old)
        .arg(st->todayHigh)
        .arg(st->todayLow);
    tipsLabel->setText(res);
  
        

    return tipsLabel;
}

const QString StockMonitorPlugin::itemContextMenu(const QString &itemKey)
{
    Q_UNUSED(itemKey);

    QList<QVariant> items;
    items.reserve(1);

    QMap<QString, QVariant> refresh;
    refresh["itemId"] = "roll";
    refresh["itemText"] = "滚动";
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
    if (menuId == "roll") {
        qDebug()<<"roll";
         QMessageBox messageBox(QMessageBox::NoIcon,
                           "开启滚动", "你确定要开启滚动吗?",
                           QMessageBox::Yes | QMessageBox::No, NULL); ;
        int result=messageBox.exec();
        switch (result)
        {
        case QMessageBox::Yes:
            qDebug()<<"Yes";
            connect(rollingTimmer,&QTimer::timeout,this,&StockMonitorPlugin::rolling);
            rollingTimmer->start(2000);
            break;
        case QMessageBox::No:
            qDebug()<<"NO";
            disconnect(rollingTimmer, 0, this, 0);
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
//        getMutiStockInfo();
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
    return ;
}


void StockMonitorPlugin::getMutiStockInfo(){
    //qDebug()<<QString("请求路径为 %1").arg(url);
   // QNetworkAccessManager *manager = new QNetworkAccessManager();
   // QNetworkRequest request;
    QString url="http://hq.sinajs.cn/list="+codes;
   ;
    request->setUrl(QUrl(url));
    // try{

    QNetworkReply *pReply = manager->get(*request);
        // 开启一个局部的事件循环，等待页面响应结束
    QEventLoop eventLoop;
    QObject::connect(manager, &QNetworkAccessManager::finished, &eventLoop, &QEventLoop::quit);
    eventLoop.exec();
        // 获取网页Body中的内容
    QByteArray bytes = pReply->readAll();
    QTextCodec *tc = QTextCodec::codecForName("GB18030");

    QString str = tc->toUnicode(bytes);

    if(!str.isEmpty()){
        QStringList list=str.split(";");
        for (int i=0;i<list.size()-1;i++){
            QString stockStr=list.at(i);
            QString name=stockStr.split("=").at(0).split("_").at(2);

            QStringList sto=stockStr.split(",");
            stockInfo *st=stocks->at(name);

            st->name=sto.at(0).split("\"").at(1);
            st->open=sto.at(1).toDouble();
            st->now=sto.at(3).toDouble();
            st->old=sto.at(2).toDouble();
            st->percent=(st->now-st->old)/st->old;
            st->todayHigh=sto.at(4).toDouble();
            st->todayLow=sto.at(5).toDouble();
           
        }
    }
   
    return ;
}



void StockMonitorPlugin::refresh(){
     // 更新内容`
    //qDebug()<<QString("请求开始时间 %1").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm::ss.zzz"));
   // getStockInfo("http://hq.sinajs.cn/list="+currentStockInfo->code,"GB18030");
    //qDebug()<<QString("请求结束时间 %1").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm::ss.zzz"));
    // parseData(res);

    getMutiStockInfo();
    auto st=stocks->at(currentStockInfo->code);
    QString flag="";
    QString style="";
    if(st->percent>=0){
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
    auto percent=QString::number(st->percent*100, 'f', 2);
    auto text=QString("%1: %2 %3 %4%").arg(st->name).arg(st->now).arg(flag).arg(percent);
    infoWidget->setTextAndStyle(text,style);


}

void StockMonitorPlugin::rolling(){

    int index=list->indexOf(currentStockInfo->code);
    if(index+1==list->size()){
        index=0;
    }else{
        index++;
    }
    qDebug()<<QString("next index  is %1").arg(index);

    currentStockInfo->code=list->at(index);

    auto st=stocks->at(currentStockInfo->code);
    QString flag="";
    QString style="";
    if(st->percent>=0){
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
    auto percent=QString::number(st->percent*100, 'f', 2);
    auto text=QString("%1: %2 %3 %4%").arg(st->name).arg(st->now).arg(flag).arg(percent);
    infoWidget->setTextAndStyle(text,style);
}