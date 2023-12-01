#include "MultithreadedTCPServer.h"
#include <QString>

ServerThread::ServerThread(qintptr socketDescriptor, QObject *parent)
    : QThread(parent), socketDescriptor(socketDescriptor)
{
}

void ServerThread::run()
{
    MyTcpSocket socket;
    if (!socket.setSocketDescriptor(socketDescriptor))
    {
        emit error(socket.error());
        return;
    }

    emit connected(&socket);

    connect(&socket, &QIODevice::readyRead, this, &ServerThread::readData);
    connect(&socket, &MyTcpSocket::disconnected, this, &ServerThread::onDisconnectedHasSocket);
    connect(this, &ServerThread::dataSend, this, &ServerThread::onDataSend);

    exec();
}



void ServerThread::onDisconnectedHasSocket()
{
    MyTcpSocket *socket = qobject_cast<MyTcpSocket *>(sender());
    if (socket)
    {
        emit disconnected(socket);
    }
}

void ServerThread::onDataSend(const QByteArray &data, MyTcpSocket *socket)
{
    socket->write(data);
}

void ServerThread::readData()
{
    MyTcpSocket *socket = qobject_cast<MyTcpSocket *>(sender());
    if (socket)
    {
        emit dataReceived(socket->readAll(), socket);
    }
}

void TcpServer::incomingConnection(qintptr socketDescriptor)
{
    ServerThread *thread = new ServerThread(socketDescriptor, this);
    connect(thread, &ServerThread::finished, /*thread, &ServerThread::deleteLater);*/[=](){
        thread->deleteLater();
    });
    connect(thread, &ServerThread::error, this, &TcpServer::threadError);
    connect(thread, &ServerThread::dataReceived, this, &TcpServer::handleData);
    connect(thread, &ServerThread::connected, this, &TcpServer::clientConnected);
    connect(thread, &ServerThread::disconnected, this, &TcpServer::clientDisconnected);

    //断开连接后，关闭该子线程
    connect(thread, &ServerThread::disconnected, this, [=](){
        thread->quit();
        thread->wait();
    });

    thread->start();
}

Server::Server(QObject *parent) : QObject(parent)
{
    server = new TcpServer(this);
    connect(server, &TcpServer::threadError, this, &Server::handleThreadError);
    connect(server, &TcpServer::handleData, this, &Server::handleData);
    connect(server, &TcpServer::clientConnected, this, &Server::clientConnected);
    connect(server, &TcpServer::clientDisconnected, this, &Server::clientDisconnected);

    if (!server->listen(QHostAddress::Any, 8080))
    {
        qDebug() << "Server could not start!";
    }
    else
    {
        qDebug() << "Server started!";
    }
}

void Server::handleThreadError(QAbstractSocket::SocketError socketError)
{
    qDebug() << "Thread error:" << socketError;
    emit signal_handleThreadError(socketError);
}

void Server::handleData(const QByteArray &data, MyTcpSocket *socket)
{
    qDebug() << QHostAddress(socket->peerAddress().toIPv4Address()).toString() + ":" + QString::number(socket->peerPort()) << "---Data received:" << data;
    emit signal_readRead(data, socket);
}

void Server::clientConnected(MyTcpSocket *socket)
{
    qDebug() << "Client connected:" << QHostAddress(socket->peerAddress().toIPv4Address()).toString() + ":" + QString::number(socket->peerPort());
    qDebug() << socket;
    clients.insert(socket,{QHostAddress(socket->peerAddress().toIPv4Address()).toString(), socket->peerPort()});

    qDebug() << "clients.size = " << clients.size();

    emit signal_clientConnected(socket);


    //////////////////////////////////////////////////////////////////////////////////
    //绑定发送数据的信号槽,多线程socket发送数据
    QByteArray data = "Your data to send";
    connect(this, &Server::signal_SendDataToClient, socket, &MyTcpSocket::onSendData);
    emit signal_SendDataToClient(data);
    disconnect(this, &Server::signal_SendDataToClient, socket, &MyTcpSocket::onSendData);
    //////////////////////////////////////////////////////////////////////////////////
}

void Server::clientDisconnected(MyTcpSocket *socket)
{
    QString clientAddress = clients.value(socket).first;
    quint16 clientPort = clients.value(socket).second;
    clients.remove(socket);
    qDebug() << "Client disconnected:" << clientAddress + ":" + QString::number(clientPort);
    qDebug() << socket;
    qDebug() << "clients.size = " << clients.size();

    emit signal_clientDisconnected(socket);
}

MyTcpSocket::MyTcpSocket(QObject *parent)
    : QTcpSocket(parent)
{

}

MyTcpSocket::~MyTcpSocket()
{

}

void MyTcpSocket::onSendData(const QByteArray &data)
{
    write(data);
    flush();
}

void MyTcpSocket::OnClose()
{
    close();
}
