
#ifndef MAINCLASS_H
#define MAINCLASS_H
#include <QObject>
#include <QCoreApplication>


class MainClass : public QObject
{
    Q_OBJECT
private:
    QCoreApplication *app;

public:
    void MyParsing(char *nomefile);
    void Position();
    void LineString();
    void LinearRing();
    void Polygon();

    explicit MainClass(QObject *parent = 0);

    void quit();

signals:

    void finished();

public slots:

    void run();

};
#endif // MAINCLASS_H
