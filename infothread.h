#ifndef INFOTHREAD_H
#define INFOTHREAD_H

#include <QThread>
#include <QString>
#include <string>
/**
 * InfoThread 是一个线程，利用自定义的信号来发送信息。
 */
class InfoThread : public QThread
{
    Q_OBJECT
public:
    explicit InfoThread();
    void sendInfo(std::string);
    void updateDownloadProcess(int process);
    void hideProcessBar();
    void showProcessBar();
    void updateRemotePath(std::string);

signals:
    void emitInfo(QString);
    void emitDownloadProcess(int);
    void emitSetDownloadProcessVisibility(bool);
    void emitUpdateRemotePath(QString);
};

#endif // INFOTHREAD_H
