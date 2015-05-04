#include <QJsonObject>
#include <QJsonValue>
#include <QFile>
#include <QDebug>
#include <QByteArray>
#include <QDirIterator>

#include "locations.h"
#include "variables.h"
#include "jack.h"


Jack::Jack()
{}

void Jack::executeJSON()
{
  QDirIterator dir_iterator(*qspGameDirectory + "json", QDirIterator::Subdirectories);

  while (dir_iterator.hasNext())
  {
    dir_iterator.next();

    QFileInfo file_info(dir_iterator.filePath());

    if (file_info.isFile())
    {
      if (file_info.suffix() == "json")
      {
        QFile file(dir_iterator.filePath());
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QByteArray json_data = file.readAll();
        file.close();

        QJsonDocument doc = QJsonDocument::fromJson(json_data);

        QStringList part = file_info.baseName().split("_");

        iterateKeys(doc, part[0].toInt());
      }
    }
  }
}

void Jack::iterateKeys(const QJsonDocument &doc, int array_indice)
{
  QJsonObject obj = doc.object();

  for (QString key : obj.keys())
  {

    if (obj.value(key).isString())
    {
      QString value = obj.value(key).toString();
      setStringVariable(key, array_indice, value);
    }
    else if (obj.value(key).isDouble() | obj.value(key).isBool())
    {
      int value = obj.value(key).toInt();
      setNumericVariable(key, array_indice, value);
    }
  }
}

void Jack::setStringVariable(QString key, int array_indice, QString value)
{
  int name_length = key.length();
  int val_length = value.length();

  QSP_CHAR var_name[name_length + 1];
  QSP_CHAR qsp_value[val_length + 1];

  key.toWCharArray(var_name);
  value.toWCharArray(qsp_value);

  var_name[name_length] = L'\0';
  qsp_value[val_length] = L'\0';

  QSPVar *var = qspVarReference(var_name, QSP_TRUE); //True = create if not found (I think)

  QSPVariant val = qspGetEmptyVariant(QSP_TRUE); //True = string type
  QSP_STR(val) = qsp_value;

  qDebug() << "Key:" << key << "value:" << value << "indice:" << array_indice;

  qspSetVarValueByReference(var, array_indice, &val);
}

void Jack::setNumericVariable(QString key, int array_indice, int value)
{
  int name_length = key.length();

  QSP_CHAR var_name[name_length + 1];

  key.toWCharArray(var_name);

  var_name[name_length] = L'\0';

  QSPVar *var = qspVarReference(var_name, QSP_TRUE); //True = create if not found (I think)

  QSPVariant val = qspGetEmptyVariant(QSP_FALSE); //False = not string type
  QSP_NUM(val) = value;

  qDebug() << "Key:" << key << "value:" << value << "indice:" << array_indice;

  qspSetVarValueByReference(var, array_indice, &val);

}

