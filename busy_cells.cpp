#include "mainwindow.h"
#include <QApplication>
#include <QProcessEnvironment>


int main(int argc, char *argv[]) {
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

    QApplication a(argc, argv);
    MainWindow window;
    window.show();
    return a.exec();
}
