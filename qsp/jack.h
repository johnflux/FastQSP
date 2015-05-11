#ifndef JACK_H
#define JACK_H

#include <QtCore/QtGlobal>
#define JACKQSP_EXPORT Q_DECL_EXPORT

#include <QString>
#include <QJsonDocument>
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

private:
  Jack() { qDebug() << "success";}
  Jack(Jack const&) = delete;
  void operator=(Jack const&) = delete;

  void iterateKeys(const QJsonDocument& doc, int array_indice);
  void setStringVariable(QString key, int array_indice, QString value);
  void setNumericVariable(QString key, int array_indice, int value);

};

#endif // JACK_H
