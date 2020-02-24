#ifndef FTPCLIENT_H
#define FTPCLIENT_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QInputDialog>
#include <QFileDialog>
#include "clientthread.h"
#include "basetitlebar.h"

namespace Ui {
class ftpClient;
}

class ClientThread;

class ftpClient : public QMainWindow
{
    Q_OBJECT

public:
    explicit ftpClient(QWidget *parent = 0);
    ~ftpClient();

private slots:
    void recvListItem(QString, QString, QString, QString);
    void recvInfo(QString);
    void recvSuccess();
    void recvClearList();
    void updateDownloadProcess(int process);
    void setProcessBarVIsibility(bool);
    void updateRemotePath(QString);
    void clientThreadRunning();
    void clientThreadStop();

    void on_connectButton_clicked();
    void on_downButton_clicked();
    void on_upButton_clicked();
    void on_remoteFileTree_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void on_renameButton_clicked();
    void on_deleteButton_clicked();
    void on_newButton_clicked();

private:
    Ui::ftpClient *ui;
    ClientThread* clientThread;
    bool connected = false; //标识当前的状态
//    QString allInfo;


//  界面优化
private:
    void initTitleBar();
    void paintEvent(QPaintEvent *event);
    void loadStyleSheet(const QString &sheetName);

private slots:
    void onButtonMinClicked();
    void onButtonRestoreClicked();
    void onButtonMaxClicked();
    void onButtonCloseClicked();

protected:
    BaseTitleBar* m_titleBar;
};



#endif // FTPCLIENT_H
