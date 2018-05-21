
#include "mainclass.h"
#include <QDebug>
#include <QJsonDocument>
#include <QFile>
#include <QJsonObject>
#include <QVariant>

MainClass::MainClass(QObject *parent) :
    QObject(parent)
{
    app = QCoreApplication::instance();
}

void MainClass::run()
{
    // qDebug() << "MainClass.Run is executing";

    quit();
}

void MainClass::quit()
{
    emit finished();
}

void MainClass::ApriFile(char *nomefile) {

    QFile file("/home/jsh/Desktop/Qt-projects/class_parsing/point.json");
    QByteArray contenuti;
    QJsonDocument doc;
    QJsonParseError errori;

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << " Errore di apertura. ";
        return;
    }
    else {
        qDebug() << "File" << nomefile << "aperto.";
        contenuti = file.readAll();
        file.close();
    }

    //trasforming .json
    doc = QJsonDocument::fromJson(contenuti, &errori);

    if (errori.error) {
        qWarning() << "Error parsing while importing the JSON document:" << errori.errorString() << "at "<< nomefile;
        return;
    }
    if (doc.isEmpty()) {
        qWarning() << "The JSON file is empty";
        return;
    }
    if (!doc.isObject()) {
        qWarning() << "The JSON data is not an object, thus it's not a valid GeoJSON. ";
        return;
    }

    //      qWarning() << " Other GeoJSON error";

    QJsonObject geoObject = doc.object(); // read json object
    QVariantMap mapObject = geoObject.toVariantMap();

    QString geojsonTypeMember[] = {"FeatureCollection",
                                   "Feature",
                                   "Point",
                                   "MultiPoint",
                                   "LineString",
                                   "MultiLineString",
                                   "Polygon",
                                   "MultiPolygon",
                                   "GeometryCollection",
                                  };

    QVariant member = mapObject.value("type");
    if (member == QVariant::Invalid)  {
        qWarning() << "Member type does not exist, thus is not a valid GeoJSON.";
        return;
    } else {
        QString pippo = member.toString();
        int len = sizeof(geojsonTypeMember)/sizeof(*geojsonTypeMember);
        bool typetrovatflag = 0;
        for (int indice=0; indice<=8; indice++){
            qDebug() << "elemento array di confronto : " << geojsonTypeMember[indice];
            qDebug() << "elemento estratto: " << pippo;
            if (pippo == geojsonTypeMember[indice]){
                qWarning() << "Member type has a " << pippo << "value";
                typetrovatflag = 1;
                break;
            } else {
                qWarning() << "Member type has \"" << pippo <<"\" as value, which is not a valid geoJSON type";
            }}
        if (!typetrovatflag) return;
    }

    qDebug() << "map dell'oggetto estratto: " << mapObject << endl;

}

