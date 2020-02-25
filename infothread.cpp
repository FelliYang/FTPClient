#include "infothread.h"

InfoThread::InfoThread()
{

}

void InfoThread::sendInfo(std::string info) {
    emit emitInfo(QString::fromStdString(info));
}
void InfoThread::updateDownloadProcess(int process){
    emit emitDownloadProcess(process);
}
void InfoThread::hideProcessBar(){
    emit emitSetDownloadProcessVisibility(false);
}
void InfoThread::showProcessBar(){
    emit emitSetDownloadProcessVisibility(true);
}
void InfoThread::updateRemotePath(std::string path){
    emit emitUpdateRemotePath(QString::fromStdString(path));
}
void InfoThread::updateTransferSpeed(int speed){
    emit emitTransferSpeed(speed);
}
void InfoThread::hideTransferSpeed(){
    emit emitSetTransferSpeedVisibility(false);
}
void InfoThread::showTransferSpeed(){
    emit emitSetTransferSpeedVisibility(true);
}
