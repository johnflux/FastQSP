#ifndef JACK_H
#define JACK_H
#include <QString>
#include <QJsonDocument>
#include "common.h"

class Jack
{
public:
  Jack();

  void executeJSON();

private:
  void iterateKeys(const QJsonDocument& doc, int array_indice);
  void setStringVariable(QString key, int array_indice, QString value);
  void setNumericVariable(QString key, int array_indice, int value);
};

#endif // JACK_H
