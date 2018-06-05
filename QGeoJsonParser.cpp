#include "QGeoJsonParser.h"
#include <QDebug>
#include <QJsonDocument>
#include <QFile>
#include <QJsonObject>
#include <QVariant>
#include <QList>
#include <QGeoPath>
#include <QGeoPolygon>

///////////////////////////////////////////////////////////////
//
//      ** PARSER **
//

QVariantMap QGeoJsonParser::geoJsonToVariantMap(QJsonObject object)
{
    QGeoJsonParser parser;
    QVariantMap errorMap;
    QVariantMap mapObject = object.toVariantMap();
    QVariantMap extractedMap;
    QString geojsonTypes[] = {"Point",
                              "MultiPoint",
                              "LineString",
                              "MultiLineString",
                              "Polygon",
                              "MultiPolygon",
                              "GeometryCollection",
                              "Feature",
                              "FeatureCollection"};
    
    enum geotypes { Point, MultiPoint, LineString, MultiLineString, Polygon, MultiPolygon, GeometryCollection, Feature, FeatureCollection };
    
    // Checking wheter JSON object has a "type" member
    QVariant typekey = mapObject.value("type");
    if (typekey == QVariant::Invalid)  {
        qWarning() << " [x] Type check failed - Invalid GeoJSON document: no \"type\" key in the object.";
        return errorMap;
    }
    
    // Converting to QString the value of the "type" member
    QString typevalue = typekey.toString();

    int len = sizeof(geojsonTypes)/sizeof(*geojsonTypes);
    int i = 0;

    // Checking whether the "type" member has a GeoJSON admitted value
    for (i = 0; i<len-1; i++){
        if (typevalue == geojsonTypes[i]){
            qWarning() << " [v] type check passed: \"type\" has a " << typevalue << "value";
            break;
        } else if (i == len-1){
            qWarning() << " [x] type check failed - Invalid GeoJSON document: \"type\" member has invalid value: \"" << typevalue <<"\"";
        }
    }

    switch (i) {  // index of the iteration where the value of type has been veryfied
    case Point:
    {
        QGeoCircle circle = parser.toPoint(mapObject);
        QVariant elementValue = QVariant::fromValue(circle);
        QString elementKey = "Point";
        extractedMap.insert(elementKey, elementValue);
        break;
    }
    case MultiPoint:
    {
        QList <QGeoCircle> multiCircle = parser.toMultiPoint(mapObject);
        QVariant elementValue = QVariant::fromValue(multiCircle); // wraps up the multiCircle item in a QVariant
        QList <QGeoCircle> testlist;
        QString elementKey = "MultiPoint"; // creating the key for the first element of the QVariantMap that will be returned
        extractedMap.insert(elementKey, elementValue); // creating the QVariantMap element
        break;
    }
    case LineString:
    {
        QGeoPath lineString = parser.toLineString(mapObject);
        QVariant elementValue = QVariant::fromValue(lineString);
        QString elementKey = "LineString";
        extractedMap.insert(elementKey, elementValue);
        break;
    }
    case MultiLineString:
    {
        QList <QGeoPath> multiLineString = parser.toMultiLineString(mapObject);
        QVariant elementValue = QVariant::fromValue(multiLineString);
        QString elementKey = "MultiLineString";
        extractedMap.insert(elementKey, elementValue);
        break;
    }
    case Polygon:
    {
        QGeoPolygon poly = parser.toPolygon(mapObject);
        QVariant elementValue = QVariant::fromValue(poly);
        QString elementKey = "Polygon";
        extractedMap.insert(elementKey, elementValue);
        break;
    }
    case MultiPolygon:
    {
        QList <QGeoPolygon> multiPoly = parser.toMultiPolygon(mapObject);
        QVariant elementValue = QVariant::fromValue(multiPoly);
        QString elementKey = "MultiPolygon";
        extractedMap.insert(elementKey, elementValue);
        break;
    }
    case GeometryCollection: // list of GeoJson geometry objects
    {
        QVariantList multigeo = parser.toGeometryCollection(mapObject);
        QVariant elementValue = QVariant::fromValue(multigeo);
        QString elementKey = "GeometryCollection";
        extractedMap.insert(elementKey, elementValue);
        break;
    }
    case Feature: // single GeoJson geometry object with properties
    {
        QVariantMap feat = parser.toFeature(mapObject);
        QVariant elementValue = QVariant::fromValue(feat);
        QString elementKey = "Feature";
        extractedMap.insert(elementKey, elementValue);
        break;
    }
    case FeatureCollection: // heterogeneous list of GeoJSON geometries with properties
    {
        QVariantList featCollection = parser.toFeatureCollection(mapObject);
        QVariant elementValue = QVariant::fromValue(featCollection);
        QString elementKey = "FeatureCollection";
        extractedMap.insert(elementKey, elementValue);
        break;
    }
    default:
    {
        qWarning() << mapObject.value("type") << "It's not a valid value";
    }
    }

    qWarning() << " Extracted geoObject" << extractedMap <<endl;

    return extractedMap;
}

QGeoCircle QGeoJsonParser::toPoint(QVariantMap point) {

    QGeoJsonParser pointConverter;
    QGeoCircle parsedPoint;
    QGeoCoordinate center;
    const QString keyPoint = "coordinates";
    QVariant valueCoords = point.value(keyPoint); // returns the value associated with the key coordinates (Point)

    center = pointConverter.coordinatesForPoint(valueCoords);
    parsedPoint.setCenter(center);
    return parsedPoint;
}

QList <QGeoCircle> QGeoJsonParser::toMultiPoint(QVariantMap multiPoint) {

    QGeoJsonParser pointConverter;
    QList <QGeoCircle> geoCircleList;
    QGeoCircle parsedPoint;
    QGeoCoordinate center;
    QString keyMultiPoint = "coordinates";
    QVariant listCoords = multiPoint.value(keyMultiPoint); // returns the value associated with the key coordinates (MultiPoint)
    QVariantList list = listCoords.value<QVariantList>(); // extracting the QVariantList from the QVariant
    QVariantList::iterator iter; // iterating MultiPoint coordinates nasted arrays

    for (iter = list.begin(); iter != list.end(); ++iter) {
        center = pointConverter.coordinatesForPoint(*iter);
        parsedPoint.setCenter(center);
        geoCircleList.append(parsedPoint); // adding the newly created QGeoCircle to the dastination QVariantList
    }
    return geoCircleList;
}

QGeoCoordinate QGeoJsonParser::coordinatesForPoint(QVariant coordinateNesting0) {

    QGeoCoordinate coord;
    QVariantList list = coordinateNesting0.value<QVariantList>();
    QVariantList::iterator iter; // iterating Point coordinates arrays

    for (iter = list.begin(); iter != list.end(); ++iter) {
        QString elementType = iter->typeName();

        switch (list.indexOf(*iter)) {
        case 0:
            coord.setLatitude(iter->toDouble());
            break;
        case 1:
            coord.setLongitude(iter->toDouble());
            break;
        case 2:
            coord.setAltitude(iter->toDouble());
            break;
        }
    }
    return coord;
}

// converts the QVariantMap obtained from the GeoJSON file to a QPath item
QGeoPath QGeoJsonParser::toLineString(QVariantMap line) {

    QGeoJsonParser lineStringConverter;
    QGeoPath parsedLineString;
    QList <QGeoCoordinate> coord2;
    const QString keyLineString = "coordinates";
    QVariant valueCoordinates = line.value(keyLineString); // returns the value associated with the key coordinates (LineString)

    coord2 = lineStringConverter.coordinatesForLineString(valueCoordinates); //method to extract an array of QGeoCoordinate from a nested GeoJSON array

    parsedLineString.setPath(coord2);
    return parsedLineString;
}

QList <QGeoCoordinate> QGeoJsonParser::coordinatesForLineString(QVariant coordinateNesting1)
{
    QGeoJsonParser multiCoordConverter;
    QVariantList list2 = coordinateNesting1.value<QVariantList>();
    QGeoCoordinate pointForMulti;
    QList <QGeoCoordinate> listForMulti;

    QVariantList::iterator iter; // iterating the LineString coordinates nasted arrays

    for (iter = list2.begin(); iter != list2.end(); ++iter) {
        pointForMulti = multiCoordConverter.coordinatesForPoint(*iter); // using coordinatesForPoint method to convert multi positions from QVariant containing QVariantList
        listForMulti.append(pointForMulti); // populating the QList of coordinates
    }
    return listForMulti;
}

QList <QGeoPath> QGeoJsonParser::toMultiLineString(QVariantMap multiLineString)
{
    QGeoJsonParser lineStringConverter;
    QList <QGeoPath> geoPathList;
    QGeoPath parsedLineString;
    QList <QGeoCoordinate> coord3;
    QString keyMultiLineString = "coordinates";
    QVariant listCoords = multiLineString.value(keyMultiLineString); // extracting the QVariant from the QVariantMap
    QVariantList list = listCoords.value<QVariantList>(); // extracting the QVariantList from the QVariant

    QVariantList::iterator iter; // iterating the MultiLineString coordinates nasted arrays using coordinatesForLineString

    for (iter = list.begin(); iter != list.end(); ++iter) {

        coord3 = lineStringConverter.coordinatesForLineString(*iter);
        parsedLineString.setPath(coord3);
        geoPathList.append(parsedLineString); // adding the newly creatid QGeoCircle to the dastination QVariantList

        // not sure if setPath overwrites the full list
        /*
        QList <QGeoCoordinate> testlist = parsedLineString.path();
        qDebug() << "testlist:"<<testlist<<endl;
        */
        // yes, it does overwrite the coordinates

    }
    return geoPathList;
}


QGeoPolygon QGeoJsonParser::toPolygon(QVariantMap poly)
{
    QGeoJsonParser polyConverter;
    QGeoPolygon parsedPoly;
    QList <QGeoCoordinate> coord4;
    const QString keyPoly = "coordinates";
    QVariant valueCoordinates = poly.value(keyPoly); // returns the value associated with the key coordinates (Polygon)
    coord4 = polyConverter.coordinatesForPolygon(valueCoordinates); //method to extract an array of QGeocoordinates from a nested GeoJSON array
    parsedPoly.setPath(coord4);
    return parsedPoly;
}

QList <QGeoCoordinate> QGeoJsonParser::coordinatesForPolygon(QVariant coordinateNesting2)
{
    QGeoJsonParser multiCoordConverter;
    QVariantList list3 = coordinateNesting2.value<QVariantList>(); // extracting the QVariantList
    QList <QGeoCoordinate> finalListForMulti;

    QVariantList::iterator iter; // iterating the Polygon coordinates nasted arrays

    // iteration of list nested is useful for internal holes when QGPolygon will support them

    for (iter = list3.begin(); iter != list3.end(); ++iter) {

        // the first list is the border of the polygon, the other lists will be interpreted as internal holes
        if (list3.indexOf(*iter) == 0)
            finalListForMulti = multiCoordConverter.coordinatesForLineString(*iter);
        // using coordinatesForLineString method to convert multiple positions from QVariant containing QVariantList
    }
    return finalListForMulti;
}


QList <QGeoPolygon> QGeoJsonParser::toMultiPolygon(QVariantMap multiPoly)
{
    QGeoJsonParser multiPolyConverter;
    QList <QGeoPolygon> geoPolyList;
    const QString keyMultiPolygon = "coordinates";
    QVariant listCoords = multiPoly.value(keyMultiPolygon);
    QList <QGeoCoordinate> pathForPolygon;
    QVariantList list = listCoords.value<QVariantList>();
    QGeoPolygon parsedPoly;

    QVariantList::iterator iterMultiPoly; // iterating the MultiPolygon coordinates nasted arrays using coordinatesForLineString
    QVariantList::iterator iterPoly;
    for (iterMultiPoly =  list.begin(); iterMultiPoly != list.end(); ++ iterMultiPoly) {
        QVariantList nestedList = iterMultiPoly->value<QVariantList>();

        for (iterPoly =  nestedList.begin(); iterPoly != nestedList.end(); ++ iterPoly) {

            pathForPolygon = multiPolyConverter.coordinatesForPolygon(*iterPoly);
            parsedPoly.setPath(pathForPolygon);

        }
        geoPolyList.append(parsedPoly);
    }
    return geoPolyList;
}

QVariantList QGeoJsonParser::toGeometryCollection(QVariantMap geometryCollection)
{
    QVariantList returnedList;

    QVariant listGeometries = geometryCollection.value("geometries");

    QVariantList list = listGeometries.value<QVariantList>();
    QVariantList::iterator iterGeometries;

    for (iterGeometries =  list.begin(); iterGeometries != list.end(); ++ iterGeometries) {
        QVariantMap geometryMap = iterGeometries->value<QVariantMap>();
        QVariantMap geoMap = geometry(geometryMap);
        returnedList.append(geoMap);
    }
    return returnedList;
}

QVariantMap QGeoJsonParser::toFeature(QVariantMap feature) {

}

QVariantList QGeoJsonParser::toFeatureCollection(QVariantMap featureCollection) {}


// object members
QVariantMap QGeoJsonParser::geometry(QVariantMap geometryMember) {

    QGeoJsonParser parser;
    QVariantMap extractedMap;
    QString geometryTypes[] = {"Point",
                               "MultiPoint",
                               "LineString",
                               "MultiLineString",
                               "Polygon",
                               "MultiPolygon",
                               "GeometryCollection"};

    enum geotypes { Point, MultiPoint, LineString, MultiLineString, Polygon, MultiPolygon, GeometryCollection };

    const QString keyGeometries = "geometries";

        QVariant geoKey = geometryMember.value("type");

        QString geoValue = geoKey.toString();
        int len = sizeof(geometryTypes)/sizeof(*geometryTypes);
        int i = 0;

        for (i = 0; i<len-1; i++) {
            if (geoValue == geometryTypes[i])
                break;
        }

        switch (i) {
        case Point:
        {
            QGeoCircle circle = parser.toPoint(geometryMember);
            QVariant geoElement = QVariant::fromValue(circle);
            QString elementKey = "Point";
            extractedMap.insert(elementKey, geoElement);
            break;
        }
        case MultiPoint:
        {
            QList <QGeoCircle> multiCircle = parser.toMultiPoint(geometryMember);
            QVariant geoElement = QVariant::fromValue(multiCircle); // wraps up the multiCircle item in a QVariant
            QList <QGeoCircle> testlist;
            QString elementKey = "MultiPoint"; // creating the key for the first element of the QVariantMap that will be returned
            extractedMap.insert(elementKey, geoElement); // creating the QVariantMap element
            break;
        }
        case LineString:
        {
            QGeoPath lineString = parser.toLineString(geometryMember);
            QVariant geoElement = QVariant::fromValue(lineString);
            QString elementKey = "LineString";
            extractedMap.insert(elementKey, geoElement);
            break;
        }
        case MultiLineString:
        {
            QList <QGeoPath> multiLineString = parser.toMultiLineString(geometryMember);
            QVariant geoElement = QVariant::fromValue(multiLineString);
            QString elementKey = "MultiLineString";
            extractedMap.insert(elementKey, geoElement);
            break;
        }
        case Polygon:
        {
            QGeoPolygon poly = parser.toPolygon(geometryMember);
            QVariant geoElement = QVariant::fromValue(poly);
            QString elementKey = "Polygon";
            extractedMap.insert(elementKey, geoElement);
            break;
        }
        case MultiPolygon:
        {
            QList <QGeoPolygon> multiPoly = parser.toMultiPolygon(geometryMember);
            QVariant geoElement = QVariant::fromValue(multiPoly);
            QString elementKey = "MultiPolygon";
            extractedMap.insert(elementKey, geoElement);
            break;
        }
        case GeometryCollection: // list of GeoJson geometry objects
        {
            QVariantList multigeo = parser.toGeometryCollection(geometryMember);
            QVariant geoElement = QVariant::fromValue(multigeo);
            QString elementKey = "GeometryCollection";
            extractedMap.insert(elementKey, geoElement);
            break;
        }
        }
        return extractedMap;
    }

QVariantMap QGeoJsonParser::properties(QVariantMap propertiesMember) {}
QVariantList QGeoJsonParser::feautures(QVariantList featuresMember) {}

///////////////////////////////////////////////////////////////
//
//    CONTAINER CLASS
//

void QGeoJsonParser::MyParsing(char *fileName)
{
    QFile file("/home/jsh/Projects/class_parsing/multipoly.json");
    QByteArray contents;
    QJsonDocument doc;
    QJsonParseError err;
    QVariant coordsextracted;
    QGeoPath extractedpath;
    
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
        qWarning() << " [x] GeoJSON validity check failed - The JSON data is not an object";
        return;
    } else qWarning() <<"[v] GeoJSON validity check passed - The JSON data is an object";
    
    // qWarning() << " other GeoJSON error";
    
    QJsonObject geoObject = doc.object(); // read json object
    QVariantMap myMap = geoJsonToVariantMap(geoObject);
    
    qWarning() << endl << "QMap extracted from the JSON object: " << endl << myMap << endl;
    
    return;
}

///////////////////////////////////////////////////////////////
//
//    SIGNALS
//

QGeoJsonParser::QGeoJsonParser(QObject *parent):QObject(parent)
{
    app = QCoreApplication::instance();
}

void QGeoJsonParser::run()
{
    // qDebug() << "QGeoJsonParser.Run is executing";
    quit();
}

void QGeoJsonParser::quit()
{
    emit finished();
}


