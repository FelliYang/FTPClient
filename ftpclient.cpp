#include <QApplication>
#include <QPainter>
#include <QDesktopWidget>
#include "ftpclient.h"
#include "ui_ftpclient.h"

ftpClient::ftpClient(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ftpClient)
{
    clientThread = new ClientThread();

    connect(clientThread, SIGNAL(emitListItem(QString, QString, QString, QString)), this, SLOT(recvListItem(QString, QString, QString, QString)));
    connect(clientThread, SIGNAL(emitSuccess()), this, SLOT(recvSuccess()));
    connect(clientThread, SIGNAL(finished()), clientThread, SLOT(stop()));
    connect(clientThread, SIGNAL(emitClearList()), this, SLOT(recvClearList()));
    connect(clientThread,SIGNAL(emitRunning()),this,SLOT(clientThreadRunning()));
    connect(clientThread,SIGNAL(emitStop()),this,SLOT(clientThreadStop()));
    connect(clientThread->curClient->infoThread, SIGNAL(emitInfo(QString)), this, SLOT(recvInfo(QString)));
    connect(clientThread->curClient->infoThread,SIGNAL(emitDownloadProcess(int)),this,SLOT(updateDownloadProcess(int)));
    connect(clientThread->curClient->infoThread,SIGNAL(emitSetDownloadProcessVisibility(bool)),this,SLOT(setProcessBarVIsibility(bool)));
    connect(clientThread->curClient->infoThread,SIGNAL(emitUpdateRemotePath(QString)),this,SLOT(updateRemotePath(QString)));

    //若执行此行，run结束后clientThread会调用析构
    //connect(clientThread, SIGNAL(finished()), clientThread, SLOT(deleteLater()));

    ui->setupUi(this);
    //设置treefile的表头
    ui->remoteFileTree->header()->resizeSection(0,50);
    ui->remoteFileTree->header()->resizeSection(1,80);
    ui->remoteFileTree->header()->resizeSection(2,150);
    ui->remoteFileTree->header()->resizeSection(3,100);
    //设置进度条
    setProcessBarVIsibility(false);
    //设置状态栏
    ui->status->setText("ready");
    ui->status->setVisible(false);


    // FramelessWindowHint属性设置窗口去除边框;
    // WindowMinimizeButtonHint 属性设置在窗口最小化时，点击任务栏窗口可以显示出原窗口;
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);
    // 设置窗口背景透明;
    setAttribute(Qt::WA_TranslucentBackground);
    // 初始化标题栏;
    initTitleBar();
}

ftpClient::~ftpClient()
{
    delete ui;
    delete clientThread;
}

//slot function------------------------------------------------
void ftpClient::on_connectButton_clicked()
{
    if(!clientThread->isRunning()){ //线程已经完成了任务
        if(!connected) { //没有连接到ftp服务器
            QString ip_addr = ui->ipEdit->text();
            QString username = ui->userEdit->text();
            QString password = ui->passEdit->text();
            clientThread->curClient->login(ip_addr, username, password);
            clientThread->task = TConnect;
            clientThread->start(); //客户端线程启动
        }
        else { //点击disconnect
            clientThread->task = TDisconnect;
            clientThread->start();
            connected = false;
            ui->connectButton->setText("连接");
        }
    }else{//线程正在运行中 正在上传或者下载
        clientThread->curClient->stopCurrentTask();
        connected = false;
        ui->connectButton->setText("连接");
        ui->downloadProgress->setVisible(false);
    }
}
void ftpClient::updateRemotePath(QString remotePath){
    ui->remotePath->setText(remotePath);
}
/**
 * @brief ftpClient::setProcessBarVIsibility
 * @param b
 * 设置是否显示下载进度条
 */
void ftpClient::setProcessBarVIsibility(bool b){
    ui->downloadProgress->setVisible(b);
}
void ftpClient::on_downButton_clicked()
{
    if(connected) {
        if(!clientThread->isRunning()){
            QTreeWidgetItem* curItem = ui->remoteFileTree->currentItem();
            QString downName;
            if(curItem)
                downName = curItem->text(3); //获取文件名，只有当被选中item的时候下载才会有效
            else{
                recvInfo("please clicked the file you want to downloaded first!\n");
                return;
            }
            if(curItem->text(0)=="d"){
                recvInfo("sorry, cannot download a dictory, select a file insteadly!\n");
                return;
            }
            QString saveDir = QFileDialog::getExistingDirectory(this, "Choose save path","C:/users/xuziyang/desktop");
            if(saveDir == NULL || saveDir.isEmpty()){
                recvInfo("you don't select the dictory which target file will be download in!\n");
                return;
            }else {
                clientThread->task = TDown;
                clientThread->arglist[0] = downName.toStdString();
                clientThread->arglist[1] = saveDir.toStdString();
                ui->downloadProgress->setValue(0);  //下载进度初始化为0
                setProcessBarVIsibility(false); //默认情况不显示进度条
                clientThread->start();
            }
        }else{
            recvInfo("some work are running! wait please.\n");
        }
    }else {
        //提示没有connect
        recvInfo("please connect first!\n");
    }
}
void ftpClient::on_upButton_clicked()
{
    if(connected){
        if(!clientThread->isRunning()){
            std::string localFile;
            localFile = QFileDialog::getOpenFileName(this, "Choose the file to upload","C:/users/xuziyang/desktop").toStdString();
            if(localFile.empty()){
                recvInfo("you don't select a file!\n"); return;
            }else{
                clientThread->task = TUp;
                clientThread->arglist[0] = localFile;
                ui->downloadProgress->setValue(0);  //上传进度初始化为0
                setProcessBarVIsibility(false); //默认情况不显示进度条
                clientThread->start();
            }
        }else{
            recvInfo("some work are running! wait please.\n");
        }
    }else {
        recvInfo("please connect first!\n");
    }
}

void ftpClient::on_renameButton_clicked()
{
    if(connected){
        if(!clientThread->isRunning()){
            QTreeWidgetItem* curItem = ui->remoteFileTree->currentItem();
            QString srcName, dstName;
            if(curItem)
                srcName = curItem->text(3);
            else{
                recvInfo("please clicked the file or dictory you want to rename first!\n");
                return;
            }
            if(srcName=="." || srcName==".."){
                recvInfo("sorry, dictory . and .. cannnot be rename\n");
                return;
            }
            dstName = QInputDialog::getText(this, "Please input a name", "New name of the file");
            if(dstName.isEmpty()){
                recvInfo("you don't give the file's new name. please try again!");
                return;
            }

            if(dstName=="." || dstName==".."){
                recvInfo("sorry, you cannot use . or .. as file's new name\n");
                return;
            }
            clientThread->arglist[0] = srcName.toStdString();
            clientThread->arglist[1] = dstName.toStdString();
            clientThread->task = TRename;
            clientThread->start();
        }else{
            recvInfo("some work are running! wait please.\n");
        }

    }
    else {
        recvInfo("please connect first!\n");
    }

}


void ftpClient::on_deleteButton_clicked()
{
    if(connected){
        if(!clientThread->isRunning()){
            QTreeWidgetItem* curItem = ui->remoteFileTree->currentItem();
            QString fname;
            if(curItem)
                fname = curItem->text(3);
            else{
                recvInfo("please clicked the file or dictory you want to delete first!\n");
                return;
            }
            clientThread->arglist[0] = fname.toStdString();
            if(fname=="." || fname==".."){
                recvInfo("sorry, you cannot delete dictory . or .. !\n");
                return;
            }
            if(curItem->text(0)=="d")
                clientThread->task = TRmd;
            else
                clientThread->task = TDele;
            clientThread->start();
        }else{
            recvInfo("some work are running! wait please.\n");
        }
    }
    else {
        recvInfo("please connect first!\n");
    }
}

void ftpClient::on_newButton_clicked()
{
    if(connected){
        if(!clientThread->isRunning()){
            QString name;
            name = QInputDialog::getText(this, "Please input a name.", "Name of new directory");
            if(name.isEmpty()){
                recvInfo("you don't give the dictory's name. please try again!");
                return;
            }
            if(name=="." || name==".."){
                recvInfo("sorry, you cannot create dictory . or .. !\n");
                return;
            }
            clientThread->arglist[0] = name.toStdString();
            clientThread->task = TMkd;
            clientThread->start();
        }else{
            recvInfo("some work are running! wait please.\n");
        }

    }
    else {
        recvInfo("please connect first!\n");
    }
}
void ftpClient::on_remoteFileTree_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    QString type = item->text(0);
    if(type!="d")
        return;
    QString file = item->text(3);
    if(!clientThread->isRunning()) {
        clientThread->arglist[0] = file.toStdString();
        clientThread->task = TCd;
        clientThread->start();
    }

}
void ftpClient::updateDownloadProcess(int process){
    ui->downloadProgress->setValue(process);
}
void ftpClient::recvListItem(QString type, QString size,QString time, QString name) {
    QTreeWidgetItem* item = new QTreeWidgetItem(ui->remoteFileTree);
    item->setText(0, type.mid(0,1));
    item->setText(1, size);
    item->setText(2, time);
    item->setText(3, name);
    ui->remoteFileTree->addTopLevelItem(item);
    //ui->remoteFileTree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void ftpClient::recvInfo(QString info) {
    //TODO:添加日志文件
    QTextCursor cursor = ui->infoEdit->textCursor();
    cursor.insertText(info);
    cursor.movePosition(QTextCursor::End);
    ui->infoEdit->setTextCursor(cursor);
}

void ftpClient::recvSuccess() {
    if(!connected) {
        connected = true;
        ui->connectButton->setText("断开");
    }
    else {
        connected = false;
        ui->connectButton->setText("连接");
    }
}

void ftpClient::recvClearList() {
    ui->remoteFileTree->clear();
}

void ftpClient::clientThreadRunning(){
    ui->status->setText("running");
    ui->status->setVisible(true);
}
void ftpClient::clientThreadStop(){
    ui->status->setText("ready");
    ui->status->setVisible(true);
}




//
//
//  界面优化
void ftpClient::initTitleBar()
{
    m_titleBar = new BaseTitleBar(this);
    m_titleBar->move(0, 0);

    m_titleBar->setBackgroundColor(5,70,85);
    m_titleBar->setTitleIcon(":/ftp.ico");
    m_titleBar->setTitleContent(QStringLiteral("FTP客户端"));
    m_titleBar->setButtonType(MIN_BUTTON);
    m_titleBar->setTitleWidth(this->width());

    connect(m_titleBar, SIGNAL(signalButtonMinClicked()), this, SLOT(onButtonMinClicked()));
    connect(m_titleBar, SIGNAL(signalButtonRestoreClicked()), this, SLOT(onButtonRestoreClicked()));
    connect(m_titleBar, SIGNAL(signalButtonMaxClicked()), this, SLOT(onButtonMaxClicked()));
    connect(m_titleBar, SIGNAL(signalButtonCloseClicked()), this, SLOT(onButtonCloseClicked()));

}

void ftpClient::paintEvent(QPaintEvent* event)
{
    //  Q_UNUSED(event);
    //  QStyleOption opt;
    //  opt.init(this);
    //  QPainter painter(this);
    //  style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    //设置背景色;
    QPainter painter(this);
    QPainterPath pathBack;
    pathBack.setFillRule(Qt::WindingFill);
    pathBack.addRoundedRect(QRect(0, 0, this->width(), this->height()), 3, 3);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.fillPath(pathBack, QBrush(QColor(215, 221, 228)));

    return QWidget::paintEvent(event);
}

void ftpClient::loadStyleSheet(const QString &sheetName)
{
    QFile file(":/Resources/" + sheetName + ".css");
    file.open(QFile::ReadOnly);
    if (file.isOpen())
    {
        QString styleSheet = this->styleSheet();
        styleSheet += QLatin1String(file.readAll());
        this->setStyleSheet(styleSheet);
    }
}

void ftpClient::onButtonMinClicked()
{
    if (Qt::Tool == (windowFlags() & Qt::Tool))
    {
        hide();    //设置了Qt::Tool 如果调用showMinimized()则窗口就销毁了？？？
    }
    else
    {
        showMinimized();
    }
}

void ftpClient::onButtonRestoreClicked()
{
    QPoint windowPos;
    QSize windowSize;
    m_titleBar->getRestoreInfo(windowPos, windowSize);
    this->setGeometry(QRect(windowPos, windowSize));
}

void ftpClient::onButtonMaxClicked()
{
    m_titleBar->saveRestoreInfo(this->pos(), QSize(this->width(), this->height()));
    QRect desktopRect = QApplication::desktop()->availableGeometry();
    QRect FactRect = QRect(desktopRect.x() - 3, desktopRect.y() - 3, desktopRect.width() + 6, desktopRect.height() + 6);
    setGeometry(FactRect);
}

void ftpClient::onButtonCloseClicked()
{
    close();
}
