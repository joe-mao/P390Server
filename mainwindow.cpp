#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    LabListen = new QLabel("监听状态：");
    LabListen->setMinimumWidth(150);
    this->ui->statusBar->addWidget(LabListen);

    LabSocketState = new QLabel("Socket状态：");
    LabSocketState->setMinimumWidth(200);
    this->ui->statusBar->addWidget(LabSocketState);

    QString localIP = getLocalIP();//本机IP
    this->setWindowTitle(this->windowTitle() + "----本机IP:" + localIP);
    this->ui->comboIP->addItem(localIP);
    tcpServer = new QTcpServer(this);
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
}

QString MainWindow::getLocalIP()
{
    //获取本机IPv4地址
    QString hostName = QHostInfo::localHostName();//本地主机名
    QHostInfo hostInfo  = QHostInfo::fromName(hostName);
    QString localIP = "";
    QList<QHostAddress> addList = hostInfo.addresses();

    if(!addList.isEmpty()){
        for(int i = 0; i < addList.count(); i++){
            QHostAddress aHost = addList.at(i);
            if(QAbstractSocket::IPv4Protocol == aHost.protocol()){
                localIP = aHost.toString();
                break;
            }
        }
    }
    return localIP;
}




MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actStart_clicked()
{
    //开始监听
    QString IP = this->ui->comboIP->currentText();//IP地址
    quint16 port = ui->spinPort->value();//端口
    QHostAddress addr(IP);
    tcpServer->listen(addr, port);//开始监听
    this->ui->plainTextEdit->appendPlainText("***开始监听...");
    this->ui->plainTextEdit->appendPlainText("***服务器地址:" + tcpServer->serverAddress().toString());
    this->ui->plainTextEdit->appendPlainText("***服务器端口:" + QString::number(tcpServer->serverPort()));
    this->ui->actStart->setEnabled(false);
    this->ui->actStop->setEnabled(true);
    LabListen->setText("监听状态:正在监听");
}

void MainWindow::onNewConnection()
{
    tcpSocket = tcpServer->nextPendingConnection();//获取socket
    connect(tcpSocket, SIGNAL(connected()), this, SLOT(onClientConnected()));
    onClientConnected();
    connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(onClientDisconnected()));
    connect(tcpSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onSocketStateChange(QAbstractSocket::SocketState)));
    onSocketStateChange(tcpSocket->state());
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(onSocketReadyRead()));
}



void MainWindow::onClientConnected()
{
    //客户端接入时
    this->ui->plainTextEdit->appendPlainText("**client socket connected");
    this->ui->plainTextEdit->appendPlainText("**peer address:" + tcpSocket->peerAddress().toString());
    this->ui->plainTextEdit->appendPlainText("**peer port：" + QString::number(tcpSocket->peerPort()));
}

void MainWindow::onClientDisconnected()
{
    //客户端断开连接时
    this->ui->plainTextEdit->appendPlainText("**client socket disconnected");
    tcpSocket->deleteLater();
}

void MainWindow::onSocketStateChange(QAbstractSocket::SocketState socketState)
{
    //socket状态发生变化时
    switch(socketState){
    case QAbstractSocket::UnconnectedState:
        LabSocketState->setText("socket状态: UnconnectedState"); break;
    case QAbstractSocket::HostLookupState:
        LabSocketState->setText("socke状态: HostLookupState"); break;
    case QAbstractSocket::ConnectingState:
        LabSocketState->setText("socket状态: ConnectingState"); break;
    case QAbstractSocket::ConnectedState:
        LabSocketState->setText("socket状态: ConnectedState"); break;
    case QAbstractSocket::BoundState:
        LabSocketState->setText("socket状态: BoundState"); break;
    case QAbstractSocket::ClosingState:
        LabSocketState->setText("socket状态：ClosingState"); break;
    case QAbstractSocket::ListeningState:
        LabSocketState->setText("socket状态: ListeningState");
    }
}

void MainWindow::on_actStop_clicked()
{
    if(tcpServer->isListening()){
        //如果tcpServer正在监听
        tcpServer->close();//停止监听
        this->ui->actStart->setEnabled(true);
        this->ui->actStop->setEnabled(false);
        LabListen->setText("监听状态:已停止监听");
    }
}

void MainWindow::on_btnSend_clicked()
{
    //发送一行字符串，已换行符结束
    QString msg = this->ui->editMsg->text();
    this->ui->plainTextEdit->appendPlainText("[out] " + msg);
    this->ui->editMsg->clear();
    this->ui->editMsg->setFocus();

    QByteArray str = msg.toUtf8();
    str.append('\n');//添加一个换行符
    tcpSocket->write(str);
}

void MainWindow::onSocketReadyRead()
{
    //读取缓冲区行文本
    while(tcpSocket->canReadLine()){
        this->ui->plainTextEdit->appendPlainText("[in] " + tcpSocket->readLine());
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    delete LabListen;
    delete LabSocketState;
    delete tcpServer;
//    delete tcpSocket;
    QMainWindow::closeEvent(event);
}
