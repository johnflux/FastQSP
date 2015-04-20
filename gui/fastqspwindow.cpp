#include "qsp_callback.h"
#include "qsp_default.h"
#include "fastqspwindow.h"
#include <QIcon>

FastQSPWindow::FastQSPWindow(QWidget *parent)
    : QMainWindow(parent), gameWidth(800), gameHeight(600),
      aspectRatio(qreal(gameWidth) / qreal(gameHeight)), scaleFactor(1),
      gameIsOpen(false), netManager() {
// Init audio
#if QT_VERSION < 0x050000
  media = new Phonon::MediaObject(this);
  audioOutput = new Phonon::AudioOutput(Phonon::MusicCategory, this);
  Phonon::createPath(media, audioOutput);
#else
  player = new QMediaPlayer();
#endif

  // Start timer
  timer.start();

  // Init view
  scene = new QGraphicsScene(this);
  graphicsView = new QGraphicsView(scene, this);
  graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  graphicsView->setUpdatesEnabled(true);

  webView = new QGraphicsWebView();
  webView->page()->setNetworkAccessManager(&netManager);
  webView->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
  scene->addItem(webView);
  scene->setBackgroundBrush(QBrush(QColor(0, 0, 0)));
  webView->setRenderHints(
      QPainter::Antialiasing | QPainter::HighQualityAntialiasing |
      QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform |
      QPainter::NonCosmeticDefaultPen);
  webView->settings()->setAttribute(QWebSettings::AutoLoadImages, true);
  webView->setAutoFillBackground(false);

  videoPlayer = new QMediaPlayer();
  videoItem = new QGraphicsVideoItem();
  videoItem->setSize(QSize(gameWidth + 4, gameHeight + 4));
  videoItem->setAspectRatioMode(Qt::KeepAspectRatioByExpanding);
  videoPlayer->setVideoOutput(videoItem);
  videoPlayer->setMuted(true);
  videoPlayer->setNotifyInterval(500);
  scene->addItem(videoItem);
  videoItem->hide();
  connect(videoPlayer, SIGNAL(positionChanged(qint64)), this, SLOT(replayVideo(qint64)));

  // Filter context menu event
  graphicsView->viewport()->installEventFilter(this);

  // Creating menu
  //-File menu--------------------------------------------------------
  QMenu *fileMenu = new QMenu("File");
  fileMenu->addAction("Open file\tCtrl+O", this, SLOT(openFileDialog()));
  QShortcut *openFile = new QShortcut(QKeySequence("Ctrl+O"), this);
  connect(openFile, SIGNAL(activated()), SLOT(openFileDialog()));

  fileMenu->addAction("Exit\tCtrl+Q", this, SLOT(close()));
  QShortcut *exit = new QShortcut(QKeySequence("Ctrl+Q"), this);
  connect(exit, SIGNAL(activated()), SLOT(close()));

  menuBar()->addMenu(fileMenu);

  //-Game menu--------------------------------------------------------
  gameMenu = new QMenu("Game");
  gameMenu->addAction("Save\tCtrl+S", this, SLOT(saveGameDialog()));
  QShortcut *save = new QShortcut(QKeySequence("Ctrl+S"), this);
  connect(save, SIGNAL(activated()), SLOT(saveGameDialog()));

  gameMenu->addAction("Load\tCtrl+L", this, SLOT(loadGameDialog()));
  QShortcut *load = new QShortcut(QKeySequence("Ctrl+L"), this);
  connect(load, SIGNAL(activated()), SLOT(loadGameDialog()));

  gameMenu->addAction("Restart\tCtrl+R", this, SLOT(restartGame()));
  QShortcut *restart = new QShortcut(QKeySequence("Ctrl+R"), this);
  connect(restart, SIGNAL(activated()), SLOT(restartGame()));

  // TODO: slows the game, move saving to diffrent thread
  autosaveAction = new QAction("Autosave", this);
  autosaveAction->setCheckable(true);
  autosaveAction->setChecked(false);
  // gameMenu->addAction(autosave);

  menuBar()->addMenu(gameMenu);
  gameMenu->setDisabled(true);

  //-Other menu-------------------------------------------------------
  QMenu *otherMenu = new QMenu("Other");
  otherMenu->addAction("Fullscreen\tAlt+Enter", this, SLOT(toggleFullscreen()));
  QShortcut *fullscreen =
      new QShortcut(QKeySequence(Qt::Key_Return + Qt::AltModifier), this);
  otherMenu->addAction("Show html", this, SLOT(showHtml()));
  connect(fullscreen, SIGNAL(activated()), SLOT(toggleFullscreen()));

  menuBar()->addMenu(otherMenu);

  //-Help menu--------------------------------------------------------
  QMenu *helpMenu = new QMenu("Help");
  helpMenu->addAction("About", this, SLOT(about()));
  menuBar()->addMenu(helpMenu);
  //------------------------------------------------------------------

  connect(webView, SIGNAL(linkClicked(const QUrl &)),
          SLOT(linkClicked(const QUrl &)), Qt::DirectConnection);
  connect(webView, SIGNAL(loadFinished(bool)), SLOT(toggleUpdate()),
          Qt::DirectConnection);
  connect(webView, SIGNAL(loadStarted()), SLOT(toggleUpdate()),
          Qt::DirectConnection);

  setCentralWidget(graphicsView);

  // Initializing QSP
  QSPInit();
  QSPCallback::QSPCallback();

  qDebug() << "QSP init finished";
}

void FastQSPWindow::loadFonts() {
  QDir fontsDir(gameDirectory + "fonts");
  fontsDir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
  fontsDir.setSorting(QDir::Size | QDir::Reversed);
  QFileInfoList fontList = fontsDir.entryInfoList();
  for (int i = 0; i < fontList.size(); ++i) {
    QFileInfo fileInfo = fontList.at(i);
    QFontDatabase::addApplicationFont(fileInfo.absoluteFilePath());
  }
}

FastQSPWindow::~FastQSPWindow() { QSPDeInit(); }

bool FastQSPWindow::eventFilter(QObject * /*obj*/, QEvent *e) {
  if (e->type() == QEvent::ContextMenu)
    return true;
  return false;
}

void FastQSPWindow::toggleUpdate() {
  graphicsView->setUpdatesEnabled(!graphicsView->updatesEnabled());
}

int FastQSPWindow::getTimeFromStart() { return timer.elapsed(); }

void FastQSPWindow::toggleFullscreen() {
  if (isFullScreen()) {
    menuBar()->show();
    showNormal();
    qDebug() << "fullscreen mode off";
  } else {
    menuBar()->hide();
    showFullScreen();
    qDebug() << "fullscreen mode on";
  }
}

void FastQSPWindow::about() {
  QLabel *about = new QLabel;
  about->setText(
      "<h2>FastQSP player " % QCoreApplication::applicationVersion() %
      "</h2><table><tr><td>Author:</td> "
      "<td>Graylor[graylor@yandex.ru]</td></tr><tr><td>Page:</td> <td><a "
      "href='https://github.com/graylor/FastQSP'>https://github.com/graylor/"
      "FastQSP</a></td></tr><tr><td>License:</td> <td>GPL v3</td></tr>");
  about->setFixedSize(300, 90);
  about->show();
}

void FastQSPWindow::openFileDialog() {
  QString filename =
      QFileDialog::getOpenFileName(this, "Open File", NULL, "QSP game (*.qsp)");
  if (!filename.isEmpty())
    openFile(filename);
}

void FastQSPWindow::saveGameDialog() {
  QString filename = QFileDialog::getSaveFileName(
      this, "Save Game", saveDir.absolutePath(), "QSP save-game (*.sav)");
  if (!filename.isEmpty() &&
      !filename.endsWith(QLatin1String(".sav"), Qt::CaseInsensitive))
    filename += QLatin1String(".sav");
  saveGame(filename);
}

void FastQSPWindow::saveGame(const QString &filename) {
  qDebug() << "Saving game to" << filename;
  if (!filename.isEmpty())
    QSPSaveGame(filename.toStdWString().c_str(), true);
}

void FastQSPWindow::loadGameDialog() {
  QFileDialog dlg;
  QString filename = dlg.getOpenFileName(
      this, "Load Game", saveDir.absolutePath(), "QSP save-game (*.sav)");
  if (!filename.isEmpty())
    loadGame(filename);
}

void FastQSPWindow::loadGame(const QString &filename) {
  qDebug() << "Loading game from" << filename;
  builder.clear();
  if (!filename.isEmpty() &&
      QSPOpenSavedGame(filename.toStdWString().c_str(), true)) {
    loadPage();
  }
}

void FastQSPWindow::restartGame() {
  QSPRestartGame(true);
  builder.clear();
  loadPage();
}

void FastQSPWindow::showHtml() {
  static QTextEdit *htmlText;
  if (!htmlText)
    htmlText = new QTextEdit();
  htmlText->setPlainText(webView->page()->mainFrame()->toHtml());
  htmlText->show();
}

void FastQSPWindow::linkClicked(const QUrl &url) {
  qDebug() << "Link clicked" << url.toString();
  if (url.toString().startsWith(QLatin1String("exec:"), Qt::CaseInsensitive)) {
    QString execStr;
    execStr = url.toString().remove(0, 5).trimmed().replace("%22", "\"");
    QSPExecString(execStr.toStdWString().c_str(), true);
  } else if (url.toString().startsWith(QLatin1String("obj:"),
                                       Qt::CaseInsensitive)) {
    bool ok = false;
    int number = 0;
    number = url.toString().remove(0, 4).trimmed().toInt(&ok);
    if (!ok) {
      qCritical() << "Unknown link format" << url.toString();
      return;
    }
    QSPSetSelObjectIndex(number, true);
  } else if (url.path() == QLatin1String("/api/closemsg")) {
    builder.hideMessage();
  } else if (url.toString().toLower() == QLatin1String("restart")) {
    restartGame();
  } else if (url.host() == QLatin1String("qspgame.local")) {
    QString path = url.path();
    path.remove(0, 1);
    qDebug() << path;
    bool ok = false;
    int number;
    number = path.toUInt(&ok);
    if (ok) {
      QSPSetSelActionIndex(number - 1, true);
      QSPExecuteSelActionCode(true);
    } else {
      qWarning() << QLatin1String("Bad link path:") << path;
    }
  } else if (url.scheme() == QLatin1String("http") ||
             url.scheme() == QLatin1String("mailto")) {
    QDesktopServices::openUrl(url);
  } else {
    qWarning() << QLatin1String("Bad link:") << url.toString();
  }
  loadPage();
}

void FastQSPWindow::playAudio(QString filename, int vol) {
  filename = filename.replace('\\', '/');
#if QT_VERSION < 0x050000
  if (QFile(filename).exists() && media->state() != Phonon::PlayingState) {
    qDebug() << "playing:" << QFileInfo(filename).filePath() << vol;
    audioOutput->setVolume(qreal(vol) / qreal(100));
    media->setCurrentSource(
        QUrl::fromLocalFile(QFileInfo(filename).filePath()));
    media->play();
  }
#else
  if (QFile(filename).exists())
  {
    if(audio[filename] == NULL || audio[filename]->state() != QMediaPlayer::PlayingState)
    {
       if (audio[filename] == NULL)
         audio[filename] = new QMediaPlayer();
       audio[filename]->setMedia(QUrl::fromLocalFile(QFileInfo(filename).absoluteFilePath()));
       audio[filename]->setVolume(vol);
       audio[filename]->play();
    }
  }
#endif
}

void FastQSPWindow::stopAudio(QString filename) {
#if QT_VERSION < 0x050000
  media->stop();
#else
  if(filename == NULL)
  {
    for(QMediaPlayer *x : audio)
        x->stop();
    return;
  }
  if(audio[filename] != NULL)
  {
    audio[filename]->stop();
  }
#endif
}

void FastQSPWindow::openFile(const QString &filename) {
  builder.clear();
  if (gameIsOpen)
    autosave();
  if (!QSPLoadGameWorld(filename.toStdWString().c_str()))
    qCritical() << QString("Could not open file: ") << filename;
  if (QSPRestartGame(QSP_TRUE)) {
    gameMenu->setEnabled(true);
    gameDirectory = QFileInfo(filename).absolutePath() + "/";
    builder.setGameDir(gameDirectory);
    netManager.setGameDirectory(gameDirectory);
    loadFonts();
    QFile configFile(gameDirectory + QLatin1String("config.xml"));
    if (configFile.open(QFile::ReadOnly)) {
      QTextStream stream(&configFile);
      QString config = stream.readLine();
      configFile.close();

      QRegExp re;
      re.setMinimal(true);

      re.setPattern("width=\"(\\d+)\"");
      if (re.indexIn(config))
        gameWidth = re.cap(1).toInt();
      else
        gameWidth = 800;
      re.setPattern("height=\"(\\d+)\"");
      if (re.indexIn(config) > 0)
        gameHeight = re.cap(1).toInt();
      else
        gameHeight = 600;
      aspectRatio = qreal(gameWidth) / qreal(gameHeight);

      re.setPattern("title=\"(.+)\"");
      if (re.indexIn(config) >= 0)
        setWindowTitle(re.cap(1));
      else
        setWindowTitle("FastQSP");

      re.setPattern("icon=\"(.+)\"");
      if (re.indexIn(config) >= 0)
        QApplication::setWindowIcon(QIcon(gameDirectory + re.cap(1)));
    }
    aspectRatio = qreal(gameWidth) / qreal(gameHeight);
    loadPage();
    webView->resize(gameWidth, gameHeight);
    resize(gameWidth, gameHeight);
    gameIsOpen = true;
    saveDir = gameDirectory + "save/";
    if (!saveDir.exists()) {
      saveDir.mkpath(".");
    }
    timer.restart();
  }
}

// That function is called by callback if isRefsresh == true
// according to the debug log that never happens. maybe I should remove it?
void FastQSPWindow::refreshView() { qDebug() << "refreshView()"; }

void FastQSPWindow::loadPage() {
  QString html = builder.getHTML();
  if(builder.thereIsAMessage == false)
  {
    maybePlayVideo(html);
  }
  webView->setHtml(html, QUrl("http://qspgame.local"));
  if (autosaveAction->isChecked())
    autosave();
}

// Ugly way of looping a video, but using a playlist
// is not gapless. videPlayer->setNotifyInterval, could be
// tweaked, if impressision is a problem.
void FastQSPWindow::replayVideo(qint64 pos)
{
  if(videoPlayer->duration() > 0)
  {
    if(videoPlayer->position() > videoPlayer->duration() - 600)
    {
      videoPlayer->setPosition(0);
    }
  }
}

void FastQSPWindow::maybePlayVideo(QString html)
{
  QRegExp imgTagRegex("\\<img[^\\>]*src\\s*=\\s*\"([^\"]*)\"[^\\>]*\\>", Qt::CaseInsensitive);
  imgTagRegex.setMinimal(true);
  QStringList urlmatches;
  QStringList imgmatches;
  int offset = 0;
  while( (offset = imgTagRegex.indexIn(html, offset)) != -1){
    offset += imgTagRegex.matchedLength();
    imgmatches.append(imgTagRegex.cap(0)); // Should hold complete img tag
    urlmatches.append(imgTagRegex.cap(1)); // Should hold only src property
  }
  bool found_image_to_overlap = false;
  foreach(QString img_src, urlmatches)
  {
    img_src = img_src.replace('\\', '/');

    if(img_src.contains("pic/girls/scene") ||
       img_src.contains("pic/girls/sex") ||
       img_src.contains("pic/girls/torture"))
      {
        found_image_to_overlap = true;
        if(videoPlayer->state() != QMediaPlayer::PlayingState)
        {
          QFileInfo current_image(img_src);
          QStringList extensions;
          extensions << ".mp4" << ".mkv" << ".wmv" << ".avi" << ".webm" << ".flv";
          QFileInfo video_file;
          foreach (QString ext, extensions)
          {
            video_file.setFile(gameDirectory + current_image.path().replace("content/pic","content/video") + "/" + current_image.baseName() + ext);
            if(video_file.exists())
            {
              videoItem->show();
              videoPlayer->setMedia(QUrl::fromLocalFile(video_file.absoluteFilePath()));
              videoPlayer->play();
              break;
            }
          }
        }
      }
  }
  if(found_image_to_overlap == false)
  {
    videoPlayer->stop();
    videoItem->hide();
  }
}

void FastQSPWindow::autosave() {
  qDebug() << "autosave:" << saveDir.absolutePath() + "/auto.sav";
  saveGame(saveDir.absolutePath() + "/auto.sav");
}

void FastQSPWindow::resizeEvent(QResizeEvent * /*event*/) {
  graphicsView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
}

void FastQSPWindow::closeEvent(QCloseEvent * /*event*/) {
  if (gameIsOpen)
    autosave();
}

void FastQSPWindow::timerEvent(QTimerEvent *event) {
  QMainWindow::timerEvent(event);
  QSPExecCounter(true);
}
