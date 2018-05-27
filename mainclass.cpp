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

// methods for checking coordinates using position

bool MainClass::ValidateGeoJSONPosition (QVariant shell) {

    const QString correctTypeL = "QVariantList";
    QVariant isType = shell.typeName();
    if (isType.toString() != correctTypeL)
    {
        qWarning() << " [x] Position check no. 1 failed - is not an array.";
        return false;
    } else {
        qWarning() << " [v] Position check no. 1 passed - is an array.";
    }

    // extracting a QVariantList from a QVariant type
    QVariantList list = shell.value<QVariantList>();
    int arrayLength = list.count();
    if (arrayLength < 2)
    {
        qWarning() << " [x] Position value check no. 2 failed - need at least 2 values.";
        return false;
    } else {
        qWarning() << " [v] Position value chack no. 2 passed - has at least 2 values";
    }

    if (arrayLength > 3)
        qWarning() << " [o] Warning! This parser processes only 3 elements on GeoJSON position array.";

    QString correctTypeD = "double";
    QVariantList::iterator iter;

    int counter = 0;
    for (iter = list.begin(); iter != list.end(); ++iter) {
        counter++;
        QString elementType = iter->typeName();
        if (elementType == correctTypeD) {
            qWarning() << " [v] Position item check - item" << counter << "passed.";
        } else {
            qWarning() << " [x] Position item check - item" << counter << "failed.";
            return false;
        }
    }
    return true;
}

// validates MultiPoint's coordinates using positions validation method

bool MainClass::ValidateMultiPointCoordinates(QVariant shell) {

    const QString pippo = "QVariantList";
    QVariant pluto = shell.typeName();
    if (pluto.toString() != pippo)
    {
        qWarning() << " [x] MultiPoint coordinates check no. 1 failed - is not an array.";
        return false;
    } else {
        qWarning() << " [v] MultiPoint coordinates check no. 1 passed - is an array.";
    }

    // extracting a QVariantList from a QVariant type
    QVariantList list = shell.value<QVariantList>();
    int arrayLength = list.count();
    if (arrayLength < 1)
    {
        qWarning() << " [x] MultiPoint coordinates check no. 2 failed - has not at least one position.";
        return false;
    } else {
        qWarning() << " [v] MultiPoint coordinates check no. 2 passed - has at least one position.";
    }

    QString correctType = "QVariantList";
    QVariantList::iterator iter;

    int counter = 0;
    for (iter = list.begin(); iter != list.end(); ++iter) {
        counter++;
        QString elementType = iter->typeName();
        if ((elementType == correctType) && ValidateGeoJSONPosition(*iter)) {
            qWarning() << " [v] Position item check" << counter << "passed.";
        } else {
            qWarning() << " [x] Position item check" << counter << "failed.";
            return false;
        }
    }
    return true;
}

// validates LinString's coordinates using positions validation method

bool MainClass::ValidateLineStringCoordinates(QVariant shell) {

    const QString pippo = "QVariantList";
    QVariant pluto = shell.typeName();
    if (pluto.toString() != pippo)
    {
        qWarning() << " [x] LineString coordinates check no. 1 failed - not an array.";
        return false;
    } else {
        qWarning() << " [v] LineString coordinates check no. 1 passed - array.";
    }

    // extracting a QVariantList from a QVariant type
    QVariantList list = shell.value<QVariantList>();
    int arrayLength = list.count();
    if (arrayLength < 2)
    {
        qWarning() << " [x] LineString coordinates check no. 1 failed - need at least 2 positions.";
        return false;
    } else {
        qWarning() << " [v] LineString coordinates check no. 1 passed - has at least 2 positions";
    }

    // not sure if is mandatory to verify the type of the 2 or more elements inside the external QVariantList shell
    // momentarily skipping this check

    QString correctType = "QVariantList";
    QVariantList::iterator iter;
    int counter = 0;
    for (iter = list.begin(); iter != list.end(); ++iter) {
        counter++;
        QString elementType = iter->typeName();
        if ((elementType == correctType) && ValidateGeoJSONPosition(*iter)) {
            qWarning() << " [v] Position check item" << counter << "passed";
        } else {
            qWarning() << " [x] Position check item" << counter << "failed";
            return false;
        }
    }
    return true;
}

void MainClass::MyParsing(char *fileName) {

    QFile file("/home/jsh/Desktop/Qt-projects/class_parsing/point.json");
    QByteArray contents;
    QJsonDocument doc;
    QJsonParseError err;

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << " [x] Error while opening file";
        return;
    }
    else {
        qWarning() << " [v] File" << fileName << "opened correctly";
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


    // GaoJSON types are not extendible as written in RFC 7946 §7

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
        qWarning() << " [x] Type check failed - Invalid GeoJSON document: no \"type\" key in the object.";
        return;
    }

    // Converting to QString the value of the "type" member

    QString typevalue = typekey.toString();

    int len = sizeof(geojsonTypes)/sizeof(*geojsonTypes);

    // Checking whether the "type" member has a GeoJSON admitted value
    int i = 0;
    for (i=0; i<len-1; i++){
        if (typevalue == geojsonTypes[i]){
            qWarning() << " [v] GeoJSON type key check passed: \"type\" has a " << typevalue << "value";
            break;
        } else if (i == 8){
            qWarning() << " [x] Invalid GeoJSON document: \"type\" member has invalid value: \"" << typevalue <<"\"";
            return;
        }
    }


    // Coordinates member check
    QVariant coordinateskey = mapObject.value("coordinates");

    if (coordinateskey == QVariant::Invalid)  {
        qWarning() << " [x] Coordinates check failed - no \"coordinates\" key in the object.";
        return;
    }

    if (ValidateMultiPointCoordinates(coordinateskey)) {
        qWarning() << "[V] PASSED ";
    } else {
        qWarning() << "[x] FAILED";
    }

    ////   (\__/)
    ////  (='.'=)
    ////  (")_(")

    switch (i) {

    case 0:
        break;

    case 1:
        break;

    case 2:
        break;

    case 3:
        break;

    case 4:
        break;

    case 5:
        break;

    case 6:
        // still to be completed
        break;

    case 7:
        // still to be completed
        break;

    case 8:
        // still to be completed
        break;

    default:
        qWarning() << mapObject.value("type") << "It's not a valid value";
        break;
    }
    qWarning() << endl << "QMap extracted from the JSON object: " << endl << mapObject << endl;
}



