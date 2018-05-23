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

void MainClass::quit() {
    emit finished();

}

void MainClass::MyParsing(char *fileName) {

    QFile file("/home/jsh/Desktop/Qt-projects/class_parsing/point.json");
    QByteArray contents;
    QJsonDocument doc;
    QJsonParseError err;

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error while opening";
        return;
    }
    else {
        qDebug() << "File" << fileName << "open correctly";
        contents = file.readAll();
        file.close();
    }

    //trasforming .json
    doc = QJsonDocument::fromJson(contents, &err);

    if (err.error) {
        qWarning() << " [x] JSON validity check failed - error parsing while importing the JSON document:" << err.errorString() << "at "<< fileName;
        return;
    } else qWarning() << " [v] JSON validity check passed";

    if (doc.isEmpty()) {
        qWarning() << " [x] JSON validity check failed - empty JSON document";
        return;
    } else qWarning() << " [v] JSON content check passed: document is not empty";

    if (!doc.isObject()) {
        qWarning() << "[x] GeoJSON validity check failed - The JSON data is not an object";
        return;
    } else "[v] GeoJSON validity check passed - The JSON data is an object";

    // qWarning() << " Other GeoJSON error";

    QJsonObject geoObject = doc.object(); // read json object

    QVariantMap mapObject = geoObject.toVariantMap();

    // GaoJSON types are not extendible as written in RFC 7946 cap. 7

    QString geojsonTypes[] = {"FeatureCollection",
                              "Feature",
                              "Point",
                              "MultiPoint",
                              "LineString",
                              "MultiLineString",
                              "Polygon",
                              "MultiPolygon",
                              "GeometryCollection",
                             };

    QString geometryTypes [] = {"Point",
                                "MultiPoint",
                                "LineString",
                                "MultiLineStng",
                                "Polygon",
                                "MultiPolygon"
                               };

    // Checking JSON object with a "type" member

    QVariant typekey = mapObject.value("type");
    if (typekey == QVariant::Invalid)  {
        qWarning() << " [x] type check failed - Invalid GeoJSON document: no \"type\" key in the object.";
        return;
    }

    // Converting to QString the value of the "type" member

    QString typevalue = typekey.toString();

    int len = sizeof(geojsonTypes)/sizeof(*geojsonTypes);

    // Checking whether the "type" member has a GeoJSON admitted value

    for (int i=0; i<len-1; i++){
        if (typevalue == geojsonTypes[i]){
            qWarning() << " [v] type check passed: \"type\" has a " << typevalue << "value";
            break;
        } else if (i == 8){
            qWarning() << " [x] type check failed - Invalid GeoJSON document: \"type\" member has invalid value: \"" << typevalue <<"\"";
            return;
        }
    }

    // Coordinates member check
    QVariant coordinateskey = mapObject.value("coordinates");

    //    switch (typevalue) {
    //    case "Point":

    if (coordinateskey == QVariant::Invalid)  {
        qWarning() << " [x] coordinates check failed - Invalid GeoJSON document: no \"coordinates\" key in the object.";
        return;
    }
    const char* pippo = " QVariantList";
    qDebug() << "tipo del valore del member coordinates" << pippo;

    if( coordinateskey.typeName() != pippo)
    {
        qWarning() << " [x] coordinates check failed - Invalid GeoJSON document: \"coordinates\" key has not an array value.";
        return;
    } else {
        qWarning() << " [v] coordinates check no. 1 passed -  \"coordinates\" key has an array value.";
    }
    if ((coordinateskey.toList()).count()<2)
        qWarning() << "Coordinates does not accept less then 2 value";
    if ((coordinateskey.toList()).count()>4)
        qWarning() << "The parser can have problems to extract data";

    //            case "MultiPoint":
    //                break;

    //            case "LineString":
    //                break;

    //            case "MultiLineString":
    //                break;

    //            case "Polygon":
    //                break;

    //            case "MultiPolygon":
    //                break;

    //            case "GeometryCollection":
    //                // still to be completed
    //                break;

    //            case "Feauture":
    //                // still to be completed
    //                break;

    //            case "FeautureCollection":
    //                // still to be completed

    //                break;

    //            default:
    //                qWarning() << mapObject.value("type") << "It's not a valid value";
    //                break;
    //            }

    qDebug() << endl << "Qmap extracted from the JSON object: " << endl <<mapObject << endl;

}


