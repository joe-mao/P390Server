#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTcpServer>
#include <QTcpSocket>
#include <QCloseEvent>
#include <QHostInfo>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QLabel * LabListen;//状态栏标签
    QLabel * LabSocketState;//状态栏标签
    QTcpServer * tcpServer;//TCP服务器
    QTcpSocket * tcpSocket;//TCP通信的Socket
    QString getLocalIP();//获取本机IP地址

protected:
    void closeEvent(QCloseEvent * event);

private slots:
    void onNewConnection();//QTcpServer的newConnection()信号
    void onSocketStateChange(QAbstractSocket::SocketState socketState);
    void onClientConnected();//Client Socket connected
    void onClientDisconnected();//Client Socket disconnected
    void onSocketReadyRead();//读取socket传入的数据


    void on_actStart_clicked();//开始监听
    void on_actStop_clicked();
    void on_btnSend_clicked();
};

#endif // MAINWINDOW_H
