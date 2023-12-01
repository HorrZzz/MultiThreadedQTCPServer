#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "senddata.h"
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    //状态栏显示信息
    QLabel* label = new QLabel(this);
    ui->statusbar->addWidget(label);
    label->setText(QString::fromUtf8("IP地址:127.0.0.1") + QString::fromUtf8("端口:8080"));
    QLabel* labelTime = new QLabel(this);
    ui->statusbar->addPermanentWidget(labelTime);
    connect(&m_timer, &QTimer::timeout, this, [=](){
        QDateTime currentTime = QDateTime::currentDateTime();

        // 格式化为字符串
        QString timeString = currentTime.toString("yyyy-MM-dd hh:mm:ss");
        labelTime->setText(timeString);
    });
    m_timer.start(1000);


    //有客户端连接
    connect(&myServer, &Server::signal_clientConnected, this, [=](MyTcpSocket* socket){
        QString clientAddress = myServer.clients.value(socket).first;
        quint16 clientPort = myServer.clients.value(socket).second;
        QListWidgetItem *item1 = new QListWidgetItem();
        //将item绑定socket的指针地址
        item1->setData(Qt::UserRole, QVariant::fromValue<MyTcpSocket*>(socket));
        // 创建复选框并将其设置为项目的小部件
        QCheckBox *checkBox = new QCheckBox();
        QString threadId = QString::number((qulonglong) socket->thread());
        checkBox->setText(QString::fromUtf8("设备名称:") + QString::fromUtf8("————【") + clientAddress +
                          QString::fromUtf8(":") + QString::number(clientPort) + QString::fromUtf8("】") +
                          QString::fromUtf8("所属线程:") + threadId);
        ui->listWidget->addItem(item1);
        ui->listWidget->setItemWidget(item1, checkBox);

        // 设置默认的复选框状态
        // item1->setCheckState(Qt::Unchecked);
    });

    //客户端断开连接
    connect(&myServer, &Server::signal_clientDisconnected, this, [=](MyTcpSocket* socket){
        for (int i = 0; i < ui->listWidget->count(); ++i) {
            QListWidgetItem *currentItem = ui->listWidget->item(i);
            QVariant data = currentItem->data(Qt::UserRole);
            if (data.isValid()) {
                if (data.value<MyTcpSocket*>() == socket) {
                    qDebug() << QString::fromUtf8("查找到该item：") << socket;
                    // 在这里可以对找到的item进行处理
                    ui->listWidget->takeItem(i);
                    QCheckBox *checkBox = dynamic_cast<QCheckBox*>(ui->listWidget->itemWidget(currentItem));
                    delete checkBox;
                    delete currentItem;
                }
            }
        }
    });


    //客户端发送信息
    connect(&myServer, &Server::signal_readRead, this, [=](const QByteArray &data, QTcpSocket *socket){
        for(int i = 0; i < ui->listWidget->count(); ++i)
        {
            QListWidgetItem *currentItem = ui->listWidget->item(i);
            QVariant Pointerdata = currentItem->data(Qt::UserRole);
            if(Pointerdata.isValid())
            {
                if(Pointerdata.value<QTcpSocket*>() == socket)
                {
                    QCheckBox *checkBox = dynamic_cast<QCheckBox*>(ui->listWidget->itemWidget(currentItem));
                    QString text = checkBox->text();
                    text = "设备名称:" + QString(data) + text.mid(text.indexOf("————【"));
                    checkBox->setText(text);
                }
            }
        }
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    // 获取所有复选框被选中的项
    QList<QListWidgetItem*> checkedItems;
    for (int i = 0; i < ui->listWidget->count(); ++i) {
        QListWidgetItem *item = ui->listWidget->item(i);
        QCheckBox *checkBox = dynamic_cast<QCheckBox*>(ui->listWidget->itemWidget(item));
        if (checkBox && checkBox->isChecked()) {
            checkedItems.append(item);
            // 执行您的其他操作，如果需要
        }
    }

    // 现在，checkedItems 中包含所有复选框被选中的项
    for (QListWidgetItem *item : checkedItems) {
        //Debug阶段
        QCheckBox *checkBox = dynamic_cast<QCheckBox*>(ui->listWidget->itemWidget(item));
        qDebug() << "Checked Item: " << checkBox->text();
        MyTcpSocket* socket = item->data(Qt::UserRole).value<MyTcpSocket*>();
        QString clientAddress = myServer.clients.value(socket).first;
        quint16 clientPort = myServer.clients.value(socket).second;
        qDebug()<<"被选中的项目:"<< clientAddress + ":" + QString::number(clientPort);

        connect(this, &MainWindow::signal_SendData, socket, &MyTcpSocket::onSendData);
    }
    QByteArray data;
    QPointer<sendData>sText = new sendData();
    sText->GetData(data);
    sText->exec();

    qDebug()<<QString::fromUtf8("继续");
    qInfo() << QString::fromUtf8("info测试");

    emit signal_SendData(data);
    for(QListWidgetItem *item : checkedItems){
        MyTcpSocket* socket = item->data(Qt::UserRole).value<MyTcpSocket*>();
        disconnect(this, &MainWindow::signal_SendData, socket, &MyTcpSocket::onSendData);
    }


}


void MainWindow::on_checkBox_stateChanged(int arg1)
{
    if(arg1 == Qt::Unchecked)
    {
        QList<QListWidgetItem*> checkedItems;
        for (int i = 0; i < ui->listWidget->count(); ++i) {
            QListWidgetItem *item = ui->listWidget->item(i);
            QCheckBox *checkBox = dynamic_cast<QCheckBox*>(ui->listWidget->itemWidget(item));
            if (checkBox) {
                checkBox->setCheckState(Qt::Unchecked);
            }
        }
    }
    else if(arg1 == Qt::Checked)
    {
        QList<QListWidgetItem*> checkedItems;
        for (int i = 0; i < ui->listWidget->count(); ++i) {
            QListWidgetItem *item = ui->listWidget->item(i);
            QCheckBox *checkBox = dynamic_cast<QCheckBox*>(ui->listWidget->itemWidget(item));
            if (checkBox) {
                checkBox->setCheckState(Qt::Checked);
            }
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    for (const auto& key : myServer.clients.keys()) {
        connect(this, &MainWindow::signal_CloseAll, key, &MyTcpSocket::OnClose);
    }
    emit signal_CloseAll();
    event->accept();
}

