/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtCore/QDateTime>
#include <QtMqtt/QMqttClient>
#include <QtWidgets/QMessageBox>

#include <QVariantList>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QString>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_client = new QMqttClient(this);

    connect(m_client, &QMqttClient::stateChanged, this, &MainWindow::updateLogStateChange);
    connect(m_client, &QMqttClient::disconnected, this, &MainWindow::brokerDisconnected);

    connect(m_client, &QMqttClient::messageReceived, this, [](const QByteArray &message, const QMqttTopicName &topic) {
        const QString content = QDateTime::currentDateTime().toString()
                + QLatin1String(" Received Topic: ")
                + topic.name()
                + QLatin1String(" Message: ")
                + message
                + QLatin1Char('\n');
        // ui->editLog->insertPlainText(content);
    });

    connect(m_client, &QMqttClient::pingResponseReceived, this, []() {
        const QString content = QDateTime::currentDateTime().toString()
                + QLatin1String(" PingResponse")
                + QLatin1Char('\n');
        //  ui->editLog->insertPlainText(content);
    });

    connect(m_client, &QMqttClient::connected, this, [this]() {
        ui->label_MqttState->setText(tr("MQTT 服务连接成功！"));
    });



    mqtt_connect();
}

MainWindow::~MainWindow()
{
    delete ui;
    qApp->quit();
    if(m_sub!=nullptr){
        delete m_sub;
    }

}

void MainWindow::updateLogStateChange()
{
    const QString content = QDateTime::currentDateTime().toString()
            + QLatin1String(": State Change")
            + QString::number(m_client->state())
            + QLatin1Char('\n');
    //ui->editLog->insertPlainText(content);
}

void MainWindow::brokerDisconnected()
{
    ui->label_MqttState->setText(tr("连接失败！"));
}

void MainWindow::setClientPort(int p)
{
    m_client->setPort(p);
}


void MainWindow::on_btn_Subcribe_clicked()
{
    QMqttSubscription* subscription = m_client->subscribe(default_topic, QoS);
    if (!subscription) {
        QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Could not subscribe. Is there a valid connection?"));
        return;
    }else{
        m_sub = subscription;
        updateStatus(m_sub->state());
        ui->label_topic->setText(m_sub->topic().filter());
        connect(m_sub, &QMqttSubscription::messageReceived, this, &MainWindow::updateMessage);
        connect(m_sub, &QMqttSubscription::stateChanged, this, &MainWindow::updateStatus);
        connect(m_sub, &QMqttSubscription::qosChanged, [this](quint8 qos) {
            ui->label_QoS->setText(QString::number(qos));
        });
    }
}

#include <QByteArray>
void MainWindow::updateMessage(const QMqttMessage &msg)
{
    QString payload = msg.payload();
    ui->label_payload->setText(payload);
    QJsonParseError *error=new QJsonParseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(payload.toUtf8());
    //判断文件是否完整
    if(error->error!=QJsonParseError::NoError)
    {
        qDebug()<<"parseJson:"<<error->errorString();
        return ;
    }
    QJsonObject obj = jsonDoc.object();        //获取对象
    qDebug() <<"object size:"<<obj.size();

    qDebug() << obj["temp"].toDouble() <<endl;
    qDebug() << obj["dist"].toDouble() <<endl;

    double temp = obj["temp"].toDouble();
    double dist =  obj["dist"].toDouble();

    ui->label_temp->setText(QString::number(temp));
    ui->label_dist->setText(QString::number(dist));
    delete error;
}

void MainWindow::updateStatus(QMqttSubscription::SubscriptionState state)
{
    switch (state) {
    case QMqttSubscription::Unsubscribed:
        ui->label_subscribr_status->setText(QLatin1String("Unsubscribed"));
        break;
    case QMqttSubscription::SubscriptionPending:
        ui->label_subscribr_status->setText(QLatin1String("Pending"));
        break;
    case QMqttSubscription::Subscribed:
        ui->label_subscribr_status->setText(tr("订阅主题成功"));
        break;
    case QMqttSubscription::Error:
        ui->label_subscribr_status->setText(QLatin1String("Error"));
        break;
    default:
        ui->label_subscribr_status->setText(QLatin1String("--Unknown--"));
        break;
    }
}

void MainWindow::mqtt_connect()
{
    setClientPort(port);
    m_client->setHostname(hostname);
    m_client->setUsername(username);
    m_client->setPassword(password);
    m_client->setClientId(clientid);
    //    m_client->setAutoKeepAlive(true);
    //    m_client->keepAlive();
    m_client->connectToHost();
    //quint8 QoS = 0;
    updateLogStateChange();
}


void MainWindow::on_btn_reconnect_clicked()
{
    mqtt_connect();
}

