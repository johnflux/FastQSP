#include "fastqspwindow.h"
#include <QApplication>
#include <QtDebug>

FastQSPWindow *qspWin;

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    if (type == QtDebugMsg)
    {
      QFile outFile("log");
      outFile.open(QIODevice::WriteOnly | QIODevice::Append);
      QTextStream ts(&outFile);
      ts << msg << endl;
    }
}
void myMessageHandler(QtMsgType type, const char *msg)
{
    QString txt;
    if (type == QtDebugMsg)
    {
        txt = QString(msg);
    }
    QFile outFile("log");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << txt << endl;
}

int main(int argc, char *argv[]) {
//      qInstallMessageHandler(myMessageOutput);
  QCoreApplication::setApplicationName("FastQSP");
  QCoreApplication::setApplicationVersion(GIT_VERSION);

  QApplication a(argc, argv);
  qspWin = new FastQSPWindow();
  qspWin->resize(975, 630);

  qspWin->show();

  if (argc > 1 && QFile(argv[1]).exists())
    qspWin->openFile(argv[1]);

  return a.exec();
}
