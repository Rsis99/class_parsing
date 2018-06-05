#include <QtCore/QCoreApplication>
#include <QTimer>
#include "QGeoJsonParser.h"
#include "QVariant"
#include "QDebug"

using namespace std;

int main(int argc, char *argv[])
{
    char name[] = "~/home/jsh/Projects/class_parsing/point.json";

    QCoreApplication app(argc, argv);

    QGeoJsonParser myMain;

    // connect up the signals
    QObject::connect(&myMain, SIGNAL(finished()), &app, SLOT(quit()));

    QObject::connect(&app, SIGNAL(aboutToQuit()), &myMain, SLOT(aboutToQuitApp()));

    // This code will start the messaging engine in QT and in
    // 10ms it will start the execution in the QGeoJsonParser.run routine;
    QTimer::singleShot(10, &myMain, SLOT(run()));

    myMain.MyParsing(name);

    return app.exec();
}

