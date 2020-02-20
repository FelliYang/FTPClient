#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <fstream>
#include <QString>
#include <algorithm>
#include <WinSock2.h>
#include "infothread.h"

const int PORT = 21;
const int BUFLEN = 1000;
const int DATABUFLEN = 1000;
const char* const DELIMITER = "\r\n"; //FTP命令结束符

class Client
{
private:
    //int executeFTPCmd(int stateCode, char* cmd, char* arg=nullptr);
    int getStateCode();
    int getPortNum();
    int getFileSize(std::string fname);
    int getRemoteFileSize(std::string fname);
    int listPwd();
    int intoPasv();
    int recvControl(int stateCode, std::string errorInfo="0");
    int executeCmd(std::string cmd);
    void removeSpace(std::string&);
    void getDownloadedLength(std::string fileName); //获取本地已下载的文件长度 用于断点续传
    void updateRemotePath();
    SOCKADDR_IN serverAddr;
    std::string ip_addr, username, password, INFO;
    char* buf = new char[BUFLEN];
    char* databuf = new char[DATABUFLEN];
    SOCKET controlSocket;
    SOCKET dataSocket;
    std::string recvInfo;
    std::string nextInfo; //JUNK
    int flag=-1; //用于标识是否关闭了全部socket并调用WSACleanup 取值为0则表示成功关闭了，否则没有关闭
    long long int downloadedFileLength=0;
    bool execute; //表示当前是否在执行任务的状态
public:
    Client();
    ~Client();
    int connectServer();
    int disconnect();
    int changeDir(std::string tardir);
    void login(QString ip_addr, QString username, QString password);
    int downFile(std::string remoteName, std::string localDir);
    int upFile(std::string localName);
    int deleteFile(std::string fname);
    int deleteDir(std::string dname);
    int rename(std::string src, std::string dst);
    int mkDir(std::string name);
    inline void stopCurrentTask() {
        execute = false;
    };
    inline bool isRuningTask(){
        return execute;
    }
    inline void startTask(){
        execute = true;
    }

    InfoThread* infoThread;
    std::string pwd;
    std::vector<std::vector<std::string>> filelist;
};

#endif // CLIENT_H
