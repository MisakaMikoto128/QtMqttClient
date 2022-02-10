# QT5 MQTT 阿里云 Demo
```c
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
```