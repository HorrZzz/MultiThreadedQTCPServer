#include "senddata.h"
#include "ui_senddata.h"

sendData::sendData(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::sendData)
{
    ui->setupUi(this);
}

sendData::~sendData()
{
    delete ui;
}

void sendData::GetData(QByteArray &data)
{
    m_data = &data;
}

void sendData::on_pushButton_clicked()
{
    *m_data = ui->plainTextEdit->toPlainText().toUtf8();
    deleteLater();
}

