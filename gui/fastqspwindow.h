#ifndef QSPVIEW_H
#define QSPVIEW_H

#include "qsp_default.h"
#include "qsp_htmlbuilder.h"
#include "local_requsts_proxy.h"

#include <qglobal.h>
#include <QApplication>
#include <QMainWindow>
#include <QResizeEvent>
#include <QMenuBar>
#include <QMenu>
#include <QFileDialog>
#include <QTextEdit>
#include <QWebView>
#include <QWebFrame>
#include <QString>
#include <QRegExp>
#include <QDebug>
#include <QGridLayout>
#include <QBoxLayout>
#include <QFontDatabase>
#include <QLabel>
#include <QGraphicsScene>
#include <QGraphicsWebView>
#include <QGraphicsView>
#include <QShortcut>
#include <QDesktopServices>
#include <QTime>
#include <QSettings>
#include <QTimer>
#include <QGraphicsTextItem>

#if QT_VERSION < 0x050000
#include <Phonon/MediaObject>
#include <Phonon/AudioOutput>
#else
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QMap>
#include "audiostream.h"
#endif
#include "jack.h"

class FastQSPWindow : public QMainWindow {
  Q_OBJECT
public:
  explicit FastQSPWindow(QWidget *parent = 0);
  ~FastQSPWindow();
  void openFile(const QString &filename);
  void playAudio(QString filename, int vol, QString flags);
  void stopAudio(QString filename);
  void refreshView();
  int getTimeFromStart();
  QSP_HTMLBuilder builder;

signals:

public slots:
  void saveGame(const QString &filename);
  void saveGameDialog();
  void openFileDialog();

protected:
  void timerEvent(QTimerEvent *event);
  void resizeEvent(QResizeEvent *event);
  bool eventFilter(QObject *, QEvent *e);
  void closeEvent(QCloseEvent *event);

private slots:
  void about();
  void showHtml();
  void autosave();
  void loadGameDialog();
  void reloadQSP();
  void loadGame(const QString &filename);
  void quicksave();
  void quickload();
  void startQuickloading();
  void restartGame();
  void linkClicked(const QUrl &url);
  void toggleFullscreen();
  void toggleUpdate();
  void replayVideo(qint64 pos);
  void saveIgnoreCRCState();
  void saveMutedState(); //Reminder: One more option to be saved: Do proper settings handling!
  void hideSaveStatus();
  void nextScreen();
  void prevScreen();
  void gotoMainScreen();

private:
  Jack *qspJack;
  QMenu *gameMenu;
  QAction *autosaveAction;
  QAction *ignoreCRCAction;
  QAction *muteAction;
  QGraphicsWebView *webView;
  QGraphicsView *graphicsView;
  QGraphicsScene *scene;
  QMediaPlayer *videoPlayer ;
  QGraphicsVideoItem *videoItem;
  QString gameDirectory;
  uint gameWidth;
  uint gameHeight;
  qreal aspectRatio;
  qreal scaleFactor;
  bool gameIsOpen;
  QStringList scanHTMLForImages(QString html);
  bool choseRandomImageFromArray(QStringList urlmatches);
  void maybePlayVideo(QString html, QStringList urlmatches);
  QSettings settings;
  QGraphicsTextItem *savestatus = new QGraphicsTextItem;
  bool replaceHTML = false;
  QString qspFilePath = "";
  QString origImage, newImage; //newImage contains a randomly chosen image,
                               //from a given JSON array. Is set from choseRandomImageFromArray.
#if QT_VERSION < 0x050000
  Phonon::MediaObject *media;
  Phonon::AudioOutput *audioOutput;
#else
  QMediaPlayer *player;
  QMap<QString, AudioStream*> audio;
#endif
  QTime timer;
  QDir saveDir;
  LocalRequestsProxy netManager;
  void loadFonts();
  void loadPage();
};

#endif // QSPVIEW_H
