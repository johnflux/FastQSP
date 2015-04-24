#ifndef AUDIOSTREAM_H
#define AUDIOSTREAM_H
#include <QMediaPlayer>
#include <QMap>
#include <QPropertyAnimation>

class AudioStream : public QMediaPlayer
{
  Q_OBJECT
public:
  AudioStream(QMap<QString, AudioStream*> *audio);
  ~AudioStream();
  bool loop = false;
  bool only_play_this = false;
  bool stop_looping = false;
  void updateFlags(QString flags);

public slots:
  void play();
  void stop();

private slots:
  void playAgain(QMediaPlayer::State);
  void stopStream();

private:
  QMap<QString, AudioStream*> *audio;
  QString m_flags;
};

#endif // AUDIOSTREAM_H
