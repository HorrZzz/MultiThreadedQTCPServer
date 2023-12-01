#ifndef SENDDATA_H
#define SENDDATA_H

#include <QDialog>

namespace Ui {
class sendData;
}

class sendData : public QDialog
{
    Q_OBJECT

public:
    explicit sendData(QWidget *parent = nullptr);
    ~sendData();

    void GetData(QByteArray& data);

private slots:
    void on_pushButton_clicked();

private:
    Ui::sendData *ui;
    QByteArray* m_data;
};

#endif // SENDDATA_H
