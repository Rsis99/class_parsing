#include <QtCore/QCoreApplication>
#include <QTimer>
#include "mainclass.h"
#include "QVariant"
#include "QDebug"

using namespace std;

int main(int argc, char *argv[])
{
    char nome[] = "~/Desktop/Qt-projects/class_parsing/polygon.json";
    QCoreApplication app(argc, argv);

    MainClass myMain;

    // connect up the signals
    QObject::connect(&myMain, SIGNAL(finished()),
             &app, SLOT(quit()));
    QObject::connect(&app, SIGNAL(aboutToQuit()),
             &myMain, SLOT(aboutToQuitApp()));

    void ApriFile(char *nome);



//    void readJson(char *fileJSON);

    // This code will start the messaging engine in QT and in
    // 10ms it will start the execution in the MainClass.run routine;
    QTimer::singleShot(10, &myMain, SLOT(run()));

    myMain.ApriFile(nome);
//    myMain.readJson(nome);

    return app.exec();
}

