#ifndef QGEOJSONPARSER_H
#define QGEOJSONPARSER_H
#include <QObject>
#include <QCoreApplication>
#include <QGeoCoordinate>
#include <QGeoCircle>
#include <QGeoPath>
#include <QGeoPolygon>

class QGeoJsonParser : public QObject
{
    Q_OBJECT

private:
    QCoreApplication *app;

public:

    // parser method

    QVariantMap geoJsonToVariantMap(QJsonObject object);

    // all members needed for exporting data

    QGeoCircle toPoint(QVariantMap point); // generates a QVariantMap with key/value: "Point"/QGeoCircle
    QGeoPath toLineString(QVariantMap lineString);
    QGeoPolygon toPolygon(QVariantMap polygon);

    QList <QGeoCircle> toMultiPoint(QVariantMap multiPoint);
    QList <QGeoPath> toMultiLineString(QVariantMap multiLineString);
    QList <QGeoPolygon> toMultiPolygon(QVariantMap multiPolygon);

    QVariantList toGeometryCollection(QVariantMap geometryCollection);

    QVariantMap toFeature(QVariantMap feature);
    QVariantList toFeatureCollection(QVariantMap featureCollection);

    QGeoCoordinate coordinatesForPoint(QVariant obtainedCoordinates); // converts a single position
    QList <QGeoCoordinate> coordinatesForLineString(QVariant obtainedCoordinates); // converts an array of positions
    QList <QGeoCoordinate> coordinatesForPolygon(QVariant obtainedCoordinates); // converts an array of array of positions

    // object members
    QVariantMap geometry(QVariantMap geometryMember);
    QVariantMap properties(QVariantMap propertiesMember);
    QVariantList geometries(QVariantList geometriesMember);
    QVariantList feautures(QVariantList featuresMember);


    void MyParsing(char *nomefile);

    explicit QGeoJsonParser(QObject *parent = 0);

    void quit();

signals:

    void finished();

public slots:

    void run();

};
#endif // QGEOJSONPARSER_H
