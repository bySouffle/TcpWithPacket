#include "mainwindow.h"
#include"tcp_core.h"
#include"tcp_client_core.h"

#include <QApplication>
#include<QCoreApplication>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
//    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();
    tcp_core t;
    tcp_client_core c;

    return a.exec();
}
