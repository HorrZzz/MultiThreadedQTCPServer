#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qglobal.h>
#include "MultithreadedTCPServer.h"
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();       //发送按钮点击事件

    void on_checkBox_stateChanged(int arg1);

signals:
    void signal_SendData(const QByteArray& data);
    void signal_CloseAll();


protected:
    void closeEvent(QCloseEvent *event)override;
private:
    Ui::MainWindow *ui;

    Server myServer;
    QTimer m_timer;
};
#endif // MAINWINDOW_H
