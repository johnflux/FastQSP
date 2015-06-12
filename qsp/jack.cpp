#include <QJsonValue>
#include <QFile>
#include <QDebug>
#include <QByteArray>
#include <QDirIterator>
#include <QJsonArray>

#include "locations.h"
#include "variables.h"
#include "jack.h"
#include "game.h"
#include "callbacks.h"
#include "objects.h"

void Jack::executeJSON()
{
  setStylesheet(); //Yeah not good place, I know.
  QDirIterator dir_iterator(*qspGameDirectory + "json", QDirIterator::Subdirectories);

  while (dir_iterator.hasNext())
  {
    dir_iterator.next();

    QFileInfo file_info(dir_iterator.filePath());

    if (file_info.isFile())
    {
      if (file_info.suffix() == "json")
      {
        if(file_info.baseName() == "save_config")
          continue;

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
//  qDebug() << "Status:";
  //  loadGameStatus();
}

QList<QString> Jack::getImageArrays(QString key)
{
  if(image_arrays.contains(key))
    return image_arrays[key];
  else
    return QList<QString>();
}

void Jack::iterateKeys(const QJsonDocument &doc, int array_indice)
{
  QJsonObject obj = doc.object();

  for (QString key : obj.keys())
  {
    if (obj.value(key).isString())
    {
      QString value = obj.value(key).toString();
      if(array_indice == 0)
      {
        if(key.at(0) != '$')
          key = "$" + key;
        setStringVariable(key, value);
      }
      else
        setStringVariable(key, array_indice, value);
    }
    else if (obj.value(key).isDouble() | obj.value(key).isBool())
    {
      int value = obj.value(key).toInt();
      if(array_indice == 0)
        setNumericVariable(key, value);
      else
        setNumericVariable(key, array_indice, value);
    }
    else if (obj.value(key).isArray())
    {
      QJsonArray img_array = obj[key].toArray();
      QVariantList list = img_array.toVariantList();

//      if(image_arrays[list[0].toString()] == NULL)
//        image_arrays[list[0].toString()] = new QList<QString>;
//      else
      image_arrays[list[0].toString()].clear();

      for (int i = 0; i < list.size(); ++i)
      {
        if(i == 0)
          setStringVariable(key, array_indice, list[0].toString());

        image_arrays[list[0].toString()].append(list[i].toString());
      }
    }
    else if (obj.value(key).isObject())
    {
      QJsonObject menu_item = obj[key].toObject();

      for (QString key : menu_item.keys())
        setStringVariable(key, 0, menu_item.value(key).toString());
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

//  qDebug() << "Key:" << key << "value:" << value << "indice:" << array_indice;

  qspSetVarValueByReference(var, array_indice, &val);
}

void Jack::setStringVariable(QString key, QString value)
{
  QSP_CHAR var_name[key.length() + 1];

  key.toWCharArray(var_name);

  var_name[key.length()] = L'\0';

  QSP_CHAR var_val[value.length() + 1];
  value.toWCharArray(var_val);

  var_val[value.length()] = L'\0';

  QSPVariant variant = qspGetEmptyVariant(QSP_TRUE); // True = string type
  QSP_STR(variant) = var_val;

  qspSetVar(var_name, &variant, '=');
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

//  qDebug() << "Key:" << key << "value:" << value << "indice:" << array_indice;

  qspSetVarValueByReference(var, array_indice, &val);

}

void Jack::setNumericVariable(QString key, int value)
{
  QSP_CHAR var_name[key.length() + 1];

  key.toWCharArray(var_name);
  var_name[key.length()] = L'\0';

  QSPVariant variant = qspGetEmptyVariant(QSP_FALSE); //False = not string type
  QSP_NUM(variant) = value;

  qspSetVar(var_name, &variant, '=');
}

void Jack::executeLocation(QString location)
{
  QSP_CHAR loc_name[location.length() + 1];
  location.toWCharArray(loc_name);
  loc_name[location.length()] = L'\0';
  qspExecLocByName(loc_name, true);
}

// When loading a game, before switching to the right location
// run some locations, to set up variables/arrays, and mimic the way
// a user clicks through the menu system.
void Jack::prepareLocations(QString current_location, int interaction_city)
{
  // Always execute the two locs
  QStringList execLoc = {"#options", "#npc_base"};

  //Depending where we are, execute these locs first
  if(current_location == "interaction_city" || current_location == "interaction_screen_city")
    execLoc.append("interaction_city");

  if(current_location == "city_screen")
    execLoc.append("city_menu_form");

//  if(current_location == "slave_stat")
//    execLoc.append("$newloc");

  for(QString s : execLoc)
  {
    // If in a submenu, in interaction_city, find the "main menu", and execute that location first.
    // This is done to copy menu options, defined in the display_options array.
    // otherwise it would result in an empty menu, when saving when in a submenu.
    if(s == "interaction_city")
    {
      if(interaction_city > 100)
      {
        int base = interaction_city;
        while(base > 100)
          base /= 10;

        qDebug() << base;

        // Switch location to "main menu"
        setNumericVariable(s, base);

        executeLocation(s);

        // Switch back to the real location id
        setNumericVariable(s, interaction_city);
      }
    }

    executeLocation(s);
  }
}

void Jack::setStylesheet()
{
  QFileInfo file_info(*qspGameDirectory + "content/stylesheet.css");

  if (file_info.isFile())
  {
    QFile file(file_info.filePath());
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    setStringVariable("STYLESHEET", 0, QString(file.readAll()));
    file.close();
  }
}

void Jack::loadGameStatus(QString filename)
{
  QSPRestartGame(QSP_TRUE);
  QJsonObject save_game = loadJSONObjFromFile(filename);

  QString qloc = "menu_form";

  int int_city = 0;

  for (QString key : save_game.keys())
  {
    if(save_game.value(key).isString())
    {
      QString val = save_game.value(key).toString();

      if(key == "current_location_name")
      {
        qloc = val;
        continue;
      }

      setStringVariable(key, val);
    }
    else
    {
      if(key == "sp_txt")
        setNumericVariable("sp_txt_is_set_from_save", 1);
      if(key == "txt")
        setNumericVariable("txt_is_set_from_save", 1);

      if(key == "interaction_city")
        int_city = save_game.value(key).toInt();

      int val = save_game.value(key).toInt();

      setNumericVariable(key, val);
    }
  }

  prepareLocations(qloc, int_city);

  if(qloc == "shop_general")
    qloc = "interaction_city";

  if(qloc == "main_screen")
    qloc = "menu_form";

  QSP_CHAR myloc[qloc.length() + 1];
  qloc.toWCharArray(myloc);
  myloc[qloc.length()] = L'\0';

  // Update current location
  qspCurLoc = qspLocIndex(myloc);

  //Refresh if needed
  if(qloc != "interaction_screen_city")
  {
    qspClearObjects(true);
    qspRefreshCurLoc(QSP_TRUE, nullptr, 0);
  }

  setNumericVariable("sp_txt_is_set_from_save", 0);
  setNumericVariable("txt_is_set_from_save", 0);
}

void Jack::saveGameStatus(QString filename)
{
  QJsonObject save_config = loadJSONObjFromFile(*qspGameDirectory + "json/save_config.json");

  QJsonObject obj;

  QString curloc = QString::fromWCharArray(qspLocs[qspCurLoc].Name);
  obj["current_location_name"] = curloc;// qspCurLoc >= 0 ? qspLocs[qspCurLoc].Name : "";

  for (QString key : save_config.keys())
  {
    QString entry = save_config.value(key).toString();

    QString type = entry.at(0) == '$' ? "string" : "int";

    QSP_CHAR var_name[entry.length() + 1];

    entry.toWCharArray(var_name);

    var_name[entry.length()] = L'\0';

    QSPVariant var_data = qspGetVar(var_name);
    if(type == "int")
      obj[entry] = var_data.Val.Num;
    else
      obj[entry] = QString::fromWCharArray(var_data.Val.Str);
  }

  QFile saveFile(filename);

  if (!saveFile.open(QIODevice::WriteOnly)) {
      qWarning("Couldn't open save file.");
      return;
  }
  QJsonDocument doc(obj);
  saveFile.write(doc.toJson());
  saveFile.close();
}

QJsonObject Jack::loadJSONObjFromFile(QString filename)
{
  QFileInfo file_info(filename);

  QFile file(file_info.absoluteFilePath());
  file.open(QIODevice::ReadOnly | QIODevice::Text);
  QByteArray json_data = file.readAll();
  file.close();

  QJsonDocument doc = QJsonDocument::fromJson(json_data);

  return doc.object();
}

int Jack::qspCurrentObjectsCount()
{
  return qspCurObjectsCount;
}

bool Jack::isGotoMainScreenAcceptable()
{
  QString curloc = QString::fromWCharArray(qspLocs[qspCurLoc].Name);
  qDebug() << curloc;

  QStringList no_escape {"achtung",
                         "main_menu",
                         "demo_intro",
                         "end_day_screen",
                         "choice_screen"};

  for(QString s : no_escape)
    if(curloc == s)
      return false;

  setNumericVariable("menu_form", 0);

  return true;
}

Jack& Jack::getInstance()
{
    static Jack instance;

    return instance;
}
