#include <QtCore/QCoreApplication>
#include <QTimer>
#include "QGeoJsonParser.h"
#include "QVariant"
#include "QDebug"

using namespace std;

int main(int argc, char *argv[])
{
    char name[] = "/home/jsh/Projects/class_parsing/polygon.json";

    QCoreApplication app(argc, argv);

    QGeoJsonParser myParser;

    // connect up the signals
    QObject::connect(&myParser, SIGNAL(finished()), &app, SLOT(quit()));

    // This code will start the messaging engine in QT and in
    // 10ms it will start the execution in the QGeoJsonParser.run routine;
    QTimer::singleShot(10, &myParser, SLOT(run()));

    myParser.provisioning(name);

    return app.exec();
}

