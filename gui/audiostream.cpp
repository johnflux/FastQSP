#include "audiostream.h"

AudioStream::AudioStream(QMap<QString, AudioStream*> *audio) :
  QMediaPlayer(new QMediaPlayer()),
  audio(audio)
{
}

AudioStream::~AudioStream()
{
}

void AudioStream::updateFlags(QString flags)
{
  if(m_flags == flags)
    return;

  m_flags = flags;

  if (flags.contains("!") || flags.contains("only"))
    only_play_this = true;
  else
    only_play_this = false;

  if (flags.contains("@") || flags.contains("loop"))
  {
    if(loop == false)
    {
      loop = true;
      connect(this, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(playAgain(QMediaPlayer::State)));
    }
  }
  else
  {
    if(loop == true)
    {
      loop = false;
      disconnect(this, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(playAgain(QMediaPlayer::State)));
    }
  }
}

void AudioStream::play()
{
  if(only_play_this)
  {
    for(AudioStream *x : *audio)
    {
      if(x != this)
      {
        if(x->state() != StoppedState)
          x->stop();
      }
    }
  }
  stop_looping = false;
  (static_cast<QMediaPlayer*>(this))->play();
}

void AudioStream::stop()
{
  stop_looping = true;
  QPropertyAnimation *fadeout = new QPropertyAnimation(this, "volume");
  fadeout->setDuration(500);
  fadeout->setStartValue(this->volume());
  fadeout->setEndValue(0);
  connect(fadeout, SIGNAL(finished()), this, SLOT(stopStream()));
  fadeout->start(QPropertyAnimation::DeleteWhenStopped);
}

void AudioStream::playAgain(State state)
{
  if (state == StoppedState)
  {
    if(!stop_looping)
    {
      play();
    }
  }
}

void AudioStream::stopStream()
{
  (static_cast<QMediaPlayer*>(this))->stop();
}

