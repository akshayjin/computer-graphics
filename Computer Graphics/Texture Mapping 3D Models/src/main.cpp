#include "mainwindow.h"
#include <QApplication>
#include <QDesktopWidget>
#include <iostream>

int main(int argc, char *argv[])
{
    if(argc<3){
        std::cout << "Format: ./TextureMapping <path-to-ply-file-1> <path-to-ply-file-2>" << std::endl;
        exit(-1);
    }

    QApplication a(argc, argv);
    MainWindow w;
    w.resize(w.sizeHint());
    int desktopArea = QApplication::desktop()->width() *
                     QApplication::desktop()->height();
    int widgetArea = w.width() * w.height();

    w.setWindowTitle("Assignment 4: Texture Mapping");

    if (((float)widgetArea / (float)desktopArea) < 0.75f)
            w.show();
        else
            w.showMaximized();

    w.init_widget(argv);

    return a.exec();
}
