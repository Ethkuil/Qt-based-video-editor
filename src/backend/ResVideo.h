#pragma once

#include <QList>
#include <QString>

#include "Resource.h"

/**
 * @brief Result Video. The video to be exported.
 * @details Saves how to produce the video through the resources.
 */

struct MaterialInUse
{
    int id;
    QString name;
    ResourceType type;
    int startTime;   // in ms. < 0 means no explicit time, like subtitles
    int endTime;     // in ms. < 0 means no explicit time, like subtitles
    int duration;    // in ms. < 0 means no explicit time, like subtitles
    QString path;    // empty if not from file
    QString command; // used for ffmpeg
};

class ResVideo
{
public:
    static ResVideo *getInstance();
    ~ResVideo();

    const QList<Resource<Video>> &getVideoList() const { return m_videoList; }
    const QList<MaterialInUse> &getMaterialList() const { return m_materialList; }
    const int getDuration() const { return m_duration; }
    const bool resVideoProduced() const { return m_isResVideoProduced; }
    const QString &getDefaultFontFile() const { return m_defaultFontFile; }

    QString produceResVideo();
    void exportVideo(int width, int height, int bitrate, QString path); // bit rate in kbps

    void addVideo(int id);
    void addVideo(int id, int index);
    void addSubtitle(int id);
    void addText(int id,
                 QString x,
                 QString y,
                 double startTimeSec,
                 double endTimeSec,
                 int fontSize = 30,
                 QString fontColor = "white",
                 QString fontFile = m_defaultFontFile);
    void addImage(int id, QString x, QString y, double startTimeSec, double endTimeSec);

    void removeVideo(int index);
    void removeVideos(int beginIndex, int count);
    void permuteVideo(int oldIndex, int newIndex);

private:
    static QString m_defaultFontFile;

    QList<Resource<Video>> m_videoList;
    QList<MaterialInUse> m_materialList;
    int m_duration; // in ms

    QString m_resVideoPath;
    bool m_isResVideoProduced;

    ResVideo();
    QStringList produceArgs();
};
