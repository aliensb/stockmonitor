#include "informationwidget.h"

struct stockInfo{
    QString name;
    double open;
    double now;
    double old;
    double percent;
};

QString doGet(QString code,const char *charset);
stockInfo parseData(QString data);



InformationWidget::InformationWidget(QWidget *parent)
    : QWidget(parent)
    , m_infoLabel(new QLabel)
    , m_refreshTimer(new QTimer(this))
    // 使用 "/home" 初始化 QStorageInfo
    // 如果 "/home" 没有挂载到一个单独的分区上，QStorageInfo 收集的数据将会是根分区的
    //, m_storageInfo(new QStorageInfo("/home"))
{

    manager = new QNetworkAccessManager;
    request=new QNetworkRequest;
    m_infoLabel->setStyleSheet("QLabel {"
                               "color: white;"
                               "}");
    m_infoLabel->setAlignment(Qt::AlignCenter);
// 如果插件没有被禁用则在初始化插件时才添加主控件到面板上
    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->addWidget(m_infoLabel);
    centralLayout->setSpacing(0);
    centralLayout->setMargin(0);

    setLayout(centralLayout);

    
}


void InformationWidget::refreshInfo()
{
    // 更新内容`
    //qDebug()<<QString("请求开始时间 %1").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm::ss.zzz"));
    QString res=doGet("http://hq.sinajs.cn/list="+plugin->currentCode,"GB18030");
    //qDebug()<<QString("请求结束时间 %1").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm::ss.zzz"));
    auto data=parseData(res);
    QString flag="";
    
    if(data.percent>=0){
         m_infoLabel->setStyleSheet("QLabel {"
                               "color: red;"
                               "padding: auto 10px;"
                               "}");
        flag="↑";
    }else{
         m_infoLabel->setStyleSheet("QLabel {"
                               "color: green;"
                               "padding: auto 10px;"
                               "}");
         flag="↓";
    }
    auto percent=QString::number(data.percent*100, 'f', 2);
    auto text=QString("%1: %2 %3 %4%").arg(data.name).arg(data.now).arg(flag).arg(percent);
    m_infoLabel->setText(text);
   // m_infoLabel->adjustSize();
   // m_infoLabel->setText(doGet("http://hq.sinajs.cn/list=sh601006","GB18030"));
}

QString InformationWidget::doGet(QString url,const char *charset){
    //qDebug()<<QString("请求路径为 %1").arg(url);
   // QNetworkAccessManager *manager = new QNetworkAccessManager();
   // QNetworkRequest request;
    request->setUrl(QUrl(url));
    QNetworkReply *pReply = manager->get(*request);
        // 开启一个局部的事件循环，等待页面响应结束
    QEventLoop eventLoop;
    QObject::connect(manager, &QNetworkAccessManager::finished, &eventLoop, &QEventLoop::quit);
    eventLoop.exec();
        // 获取网页Body中的内容
    QByteArray bytes = pReply->readAll();
    QTextCodec *tc = QTextCodec::codecForName(charset);

    QString str = tc->toUnicode(bytes);


    return str;
}

 void InformationWidget::regist(HomeMonitorPlugin *plg){
     this->plugin=plg;
     // 连接 Timer 超时的信号到更新数据的槽上
    connect(m_refreshTimer, &QTimer::timeout, this, &InformationWidget::refreshInfo);

    // 设置 Timer 超时为 10s，即每 10s 更新一次控件上的数据，并启动这个定时器
    m_refreshTimer->start(500);

    refreshInfo();
 }

stockInfo parseData(QString data){
     QStringList list=data.split(",");
     stockInfo st;
    //qDebug()<<QString("数据长度为 %1").arg(list.size());
     if(list.size()<4){
         return st;
     }
     st.name=list.at(0).split("\"").at(1);
     st.open=list.at(1).toDouble();
     st.now=list.at(3).toDouble();
     st.old=list.at(2).toDouble();
     st.percent=(st.now-st.old)/st.old;
     return st;
}
QString doGet(QString url,const char *charset){
    
    QNetworkAccessManager *manager = new QNetworkAccessManager();
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    QNetworkReply *pReply = manager->get(request);
        // 开启一个局部的事件循环，等待页面响应结束
    QEventLoop eventLoop;
    QObject::connect(manager, &QNetworkAccessManager::finished, &eventLoop, &QEventLoop::quit);
    eventLoop.exec();
        // 获取网页Body中的内容
    QByteArray bytes = pReply->readAll();
    QTextCodec *tc = QTextCodec::codecForName(charset);

    QString str = tc->toUnicode(bytes);
    return str;
}