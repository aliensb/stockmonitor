#ifndef INFORMATIONWIDGET_H
#define INFORMATIONWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QTimer>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QVBoxLayout>
#include <QDebug>
#include <QEventLoop>
#include <QTextCodec>
#include "homemonitorplugin.h"
class HomeMonitorPlugin;
class InformationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit InformationWidget(QWidget *parent = nullptr);
    void regist(HomeMonitorPlugin *p);
    void refreshInfo();
private slots:
    // 用于更新数据的槽函数
    
    QString doGet(QString url,const char *charset);
    

private:
    // 真正的数据显示在这个 Label 上
    QLabel *m_infoLabel;
    // 处理时间间隔的计时器
    QTimer *m_refreshTimer;


    HomeMonitorPlugin *plugin;
    // 分区数据的来源
    //QStorageInfo *m_storageInfo;
    QNetworkAccessManager *manager;
    QNetworkRequest *request;
};

#endif // INFORMATIONWIDGET_H