#ifndef INFORMATIONWIDGET_H
#define INFORMATIONWIDGET_H

#include <QWidget>

#include <QVBoxLayout>
#include <QDebug>
#include <QEventLoop>
#include <QTextCodec>
#include <QLabel>

class InformationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit InformationWidget(QWidget *parent = nullptr);
// private slots:
//     // 用于更新数据的槽函数
    
//     QString doGet(QString url,const char *charset);
    
    void setTextAndStyle(QString &text,QString &style);
private:
    // 真正的数据显示在这个 Label 上
    QLabel *m_infoLabel;

};

#endif // INFORMATIONWIDGET_H