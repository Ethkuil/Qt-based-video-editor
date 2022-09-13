#pragma once
#include <QListWidget>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVector>

#include "backend/Resource.h"

class VideoListManager : public QObject
{
    Q_OBJECT

public:
    VideoListManager(QListWidget *videoList, QObject *parent = nullptr);
    ~VideoListManager();

public:
    /// @return the first video id if any video is selected, otherwise -1
    int getFirstSelectedVideoId();
    QString getFirstSelectedVideoPath();
    const Resource<Video> *getFirstSelectedVideoResource();

    QVector<int> getSelectedVideoIds();
    const QVector<const Resource<Video> *> getSelectedVideoResources();

public slots:
    void updateVideoList();

private:
    QListWidget *m_videoListWidget;

    void appendVideo(const Resource<Video> &resource);
    void clearVideoList();
};