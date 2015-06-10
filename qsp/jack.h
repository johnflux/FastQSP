#ifndef JACK_H
#define JACK_H

#include <QtCore/QtGlobal>
#define JACKQSP_EXPORT Q_DECL_EXPORT

#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMap>
#include <QList>
#include "common.h"
#include <QDebug>

class JACKQSP_EXPORT Jack
{
public:
  JACKQSP_EXPORT static Jack *getInstance()
  {
    static Jack instance;

    return &instance;
  }

  void executeJSON();
  JACKQSP_EXPORT QMap<QString, QList<QString>*> image_arrays;
  JACKQSP_EXPORT int qspCurrentObjectsCount();
  JACKQSP_EXPORT bool isGotoMainScreenAcceptable();

public slots:
  JACKQSP_EXPORT void saveGameStatus(QString filename);
  JACKQSP_EXPORT void loadGameStatus(QString filename);

private:
  Jack() { qDebug() << "success";}
  Jack(Jack const&) = delete;
  void operator=(Jack const&) = delete;

  void iterateKeys(const QJsonDocument& doc, int array_indice);
  void setStringVariable(QString key, int array_indice, QString value);
  void setStringVariable(QString key, QString value);
  void setNumericVariable(QString key, int array_indice, int value);
  void setNumericVariable(QString key, int value);
  void executeLocation(QString location);
  void prepareLocations(QString current_location, int interaction_city);
  void setStylesheet();
  QJsonObject loadJSONObjFromFile(QString filename);

};

#endif // JACK_H
