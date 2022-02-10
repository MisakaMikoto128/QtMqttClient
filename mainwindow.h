#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QtMqtt/QMqttClient>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void setClientPort(int p);

private slots:

    void updateLogStateChange();

    void brokerDisconnected();


    void on_btn_Subcribe_clicked();

public slots:
    void updateMessage(const QMqttMessage &msg);
    void updateStatus(QMqttSubscription::SubscriptionState state);

private:
    Ui::MainWindow *ui;
    QMqttClient *m_client = nullptr;
    QMqttSubscription *m_sub = nullptr;


    /*
        MQTT三元组，注意阿里云的话需要指定的ClientID
    */
    QString hostname = "xxxxx.xxxx.aliyuncs.com";
    QString username = "xxxx&xxxx";
    QString password = "xxxxx";
    QString clientid = "xxxx|securemode=x,signmethod=xxx,timestamp=xxx|";
    int port = 1883;
    qint8 QoS = 0;
    QString default_topic = "/xxxx/xxxxx/";

};
#endif // MAINWINDOW_H
