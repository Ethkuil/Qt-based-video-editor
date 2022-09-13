#pragma once
#include <QMediaPlayer>
#include <QObject>
#include <QTimer>
#include <QVideoWidget>

#include "ui_QtBasedVideoEditor.h"

class PreviewWindow : public QObject
{
    Q_OBJECT

public:
    PreviewWindow(Ui_QtBasedVideoEditor *ui, QObject *parent = nullptr);
    ~PreviewWindow();

public slots:
    void onLoadVideo(QString path);
    void onPlayPause();
    void setPosition(int position);

private:
    Ui_QtBasedVideoEditor *m_ui;
    QMediaPlayer *m_player;
    QVideoWidget *m_videoWidget;
    bool m_isPlaying;
    QTimer *m_timer;
};