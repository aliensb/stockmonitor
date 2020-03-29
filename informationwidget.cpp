#include "informationwidget.h"



InformationWidget::InformationWidget(QWidget *parent)
    : QWidget(parent)
    , m_infoLabel(new QLabel)
    // 使用 "/home" 初始化 QStorageInfo
    // 如果 "/home" 没有挂载到一个单独的分区上，QStorageInfo 收集的数据将会是根分区的
    //, m_storageInfo(new QStorageInfo("/home"))
{

   
    m_infoLabel->setStyleSheet("QLabel {"
                               "color: white;"
                               "}");
    m_infoLabel->setAlignment(Qt::AlignCenter);
// 如果插件没有被禁用则在初始化插件时才添加主控件到面板上
    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->addWidget(m_infoLabel);
    m_infoLabel->setText("hello");
    centralLayout->setSpacing(0);
    centralLayout->setMargin(0);

    setLayout(centralLayout);

    
}

void InformationWidget::setTextAndStyle(QString &text,QString &style){
     m_infoLabel->setText(text);
     m_infoLabel->setStyleSheet(style);
 }