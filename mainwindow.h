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

    void on_btn_reconnect_clicked();

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
    QString hostname = "a1SHqmHCWLz.iot-as-mqtt.cn-shanghai.aliyuncs.com";
    QString username = "qt_clinet&a1SHqmHCWLz";
    QString password = "7438f5d42cea016b4224596a37323b72316e96ee";
    QString clientid = "FESA234FBDS24AA|securemode=3,signmethod=hmacsha1,timestamp=789|";
    int port = 1883;
    qint8 QoS = 0;
    QString default_topic = "/a1SHqmHCWLz/qt_clinet/user/get";

    void mqtt_connect();

};
#endif // MAINWINDOW_H
