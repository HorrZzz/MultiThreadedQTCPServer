#ifndef MULTITHREADEDTCPSERVER_H
#define MULTITHREADEDTCPSERVER_H
#include <QtWidgets>
#include <QtNetwork>
#include <QMap>

class MyTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    MyTcpSocket(QObject *parent = nullptr);
    ~MyTcpSocket();

public slots:
    void onSendData(const QByteArray &data);        //用于接受需要发送数据的槽函数
    void OnClose();     //关闭socket链接的槽函数

};



/*
 * 多线程Socket类，继承自QThread多线程函数，在run函数内实现子线程创建调用
 */
class ServerThread : public QThread
{
    Q_OBJECT
public:
    explicit ServerThread(qintptr socketDescriptor, QObject *parent = nullptr);

protected:
    void run() override;    //创建子线程，并在子线程内分配Socket与客户端链接

signals:
    void error(MyTcpSocket::SocketError socketError);
    void dataReceived(const QByteArray &data, MyTcpSocket *socket);      //接收到消息
    void dataSend(const QByteArray &data, MyTcpSocket *socket);
    void connected(MyTcpSocket *socket);
    void disconnected(MyTcpSocket *socket);  //客户端断开连接
public slots:
    void onDisconnectedHasSocket();   //Socket断开连接

    void onDataSend(const QByteArray &data, MyTcpSocket *socket);

private slots:
    void readData();    //读取客户端发送的数据
private:
    qintptr socketDescriptor;
};


/*
 * TcpServer类，继承自QTcpServer
 * 有客户端接入时，调用ServerThread创建一个子线程里的Socket给接入的客户端使用
 */
class TcpServer : public QTcpServer
{
    Q_OBJECT
public:
    TcpServer(QObject *parent = nullptr) : QTcpServer(parent) {}

protected:
    void incomingConnection(qintptr socketDescriptor) override;

signals:
    void threadError(MyTcpSocket::SocketError socketError);
    void handleData(const QByteArray &data,MyTcpSocket * socket);
    void clientConnected(MyTcpSocket *socket);
    void clientDisconnected(MyTcpSocket *socket);


};


/*
 * Server类，创建TcpServer
 */
class Server : public QObject
{
    Q_OBJECT
public:
    Server(QObject *parent = nullptr);
    QMap<MyTcpSocket *, QPair<QString, quint16>> clients;

private slots:
    void handleThreadError(MyTcpSocket::SocketError socketError);        //子线程错误

    void handleData(const QByteArray &data, MyTcpSocket *socket);        //获取数据

    void clientConnected(MyTcpSocket *socket);           //客户端加入连接
    void clientDisconnected(MyTcpSocket *socket);        //客户端断开连接


signals:
    void signal_handleThreadError(MyTcpSocket::SocketError socketError);     //子线程出错时触发
    void signal_readRead(const QByteArray &data, MyTcpSocket *socket);       //客户端发送信息时触发
    void signal_clientConnected(MyTcpSocket *socket);            //客户端连接时触发
    void signal_clientDisconnected(MyTcpSocket *socket);         //客户端断开时触发

    void signal_SendDataToClient(const QByteArray &data);   //用于给指定的socket发送数据，直接emit该信号即可

private:
    TcpServer *server;

};

#endif // MULTITHREADEDTCPSERVER_H
