#ifndef CLIENTTHREAD_H
#define CLIENTTHREAD_H

#include "client.h"

enum subThreadTask{TConnect, TDisconnect, TCd, TDown, TUp, TDele, TRmd, TRename, TMkd};

class ClientThread : public QThread
{
    Q_OBJECT
public:
    explicit ClientThread();
    ~ClientThread();
    void bind(Client *c);
    subThreadTask task;
    std::vector<std::string> arglist;
    Client* curClient;

protected:
    void run();
private:
    void flushList();
public slots:
    void stop();
signals:
    void emitListItem(QString, QString, QString, QString);
    void emitInfo(QString);
    void emitSuccess();
    void emitClearList();
    void emitRunning();
    void emitStop();

};


#endif // CLIENTTHREAD_H
