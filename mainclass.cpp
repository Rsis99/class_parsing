/*  */


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

        QFile file("/home/jsh/Desktop/Qt-projects/class_parsing/polygon.json");
        QByteArray contenuti;
        QJsonDocument doc;
        QJsonParseError *errori = new QJsonParseError();

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << " Errore di apertura. ";
            return;
        }
        else {
            qDebug() << " File aperto. ";
        }

        contenuti = file.readAll();
        file.close();

        qDebug() << contenuti;
        doc = QJsonDocument::fromJson(contenuti, errori);
        QJsonObject oggettoGJson = doc.object();
        QVariantMap mapGJson = oggettoGJson.toVariantMap();
        qDebug() << "mappa dell'oggetto estratto: " << mapGJson;
        qDebug() << "l'oggetto contiene la chiave type: " << oggettoGJson.contains("type");
        qDebug() << "è vuoto: " << doc.isEmpty();
        qDebug() << "è oggetto: " << doc.isObject();
        qDebug() << "errore in fase di importazione del documento JSON: " << errori->error;
    }

