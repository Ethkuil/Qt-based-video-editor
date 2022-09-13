#include "ResVideo.h"

using namespace std;

#include <QDebug>
#include <QDir>
#include <QProcess>
#include <QString>
#include <QStringList>

#include "Resource.h"
#include "Resources.h"

QString ResVideo::m_defaultFontFile = "C:/Windows/Fonts/msyh.ttc";

ResVideo *ResVideo::getInstance()
{
    static ResVideo *instance = new ResVideo();
    return instance;
}

ResVideo::ResVideo()
    : m_isResVideoProduced(false)
    , m_duration(0)
{}

ResVideo::~ResVideo() {}

QStringList ResVideo::produceArgs()
{
    // add input files
    QStringList args;
    for (auto &video : m_videoList) {
        args << "-i" << video.info->path.replace("\\", "/");
    }
    for (auto &material : m_materialList) {
        if (material.type == ResourceType::IMAGE) {
            args << "-i" << material.path.replace("\\", "/");
        }
    }
    const int materialCount = m_materialList.size();
    // merge videos
    args << "-filter_complex"
         << "concat=n=" + QString::number(m_videoList.size()) + ":v=1:a=1";
    // apply materials
    QString &filterComplex = args.last();
    for (int i = 0; i < materialCount; ++i) {
        filterComplex += "[v" + QString::number(i) + "]"; // mark output for last operation
        filterComplex += ";";
        filterComplex += "[v" + QString::number(i) + "]"; // take output as input
        filterComplex += m_materialList[i].command;
    }
    qDebug() << "args:" << args;
    return args;
}

QString ResVideo::produceResVideo()
{
    if (m_isResVideoProduced) {
        return m_resVideoPath;
    }

    QStringList args = produceArgs();
    QDir dir("tmp");
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    m_resVideoPath = "./tmp/resVideo.mp4";
    args << "-y" << m_resVideoPath;
    QProcess process;
    process.start("ffmpeg", args);
    if (!process.waitForStarted()) {
        qDebug() << "error:" << process.errorString();
        return QString();
    }
    process.waitForFinished();

    m_isResVideoProduced = true;
    return m_resVideoPath;
}

void ResVideo::exportVideo(int width, int height, int bitrate, QString path)
{
    QStringList args = produceArgs();
    args << "-s" << QString::number(width) + "x" + QString::number(height);
    args << "-b:v" << QString::number(bitrate) + "k";
    args << "-y" << path;
    QProcess process;
    process.start("ffmpeg", args);
    if (!process.waitForStarted()) {
        qDebug() << "error:" << process.errorString();
        return;
    }
    process.waitForFinished();
}

void ResVideo::addVideo(int id)
{
    auto resourse = Resources<Video>::getInstance()->getResource(id);
    m_videoList.push_back(resourse);
    m_duration += resourse.info->duration;
    m_isResVideoProduced = false;
}

void ResVideo::addVideo(int id, int index)
{
    auto resourse = Resources<Video>::getInstance()->getResource(id);
    m_videoList.insert(m_videoList.begin() + index, resourse);
    m_duration += resourse.info->duration;
    m_isResVideoProduced = false;
}

void ResVideo::addSubtitle(int id)
{
    MaterialInUse material;
    material.id = id;
    material.type = ResourceType::SUBTITLE;
    material.startTime = -1; // no explicit time
    material.endTime = -1;
    material.duration = -1;
    auto resourse = Resources<Material>::getInstance()->getResource(id);
    material.name = resourse.info->name;
    material.path = resourse.info->path;
    // ensure the path is valid in ffmpeg
    material.command = QString()
                           .append("subtitles=")
                           .append("\\\'") // ' -> \'
                           .append(material.path.replace("\\", "/"))
                           .append("\\\'");
    m_materialList.push_back(material);
    m_isResVideoProduced = false;
}

void ResVideo::addText(int id,
                       QString x,
                       QString y,
                       double startTimeSec,
                       double endTimeSec,
                       int fontSize,
                       QString fontColor,
                       QString fontFile)
{
    MaterialInUse material;
    material.id = id;
    material.type = ResourceType::TEXT;
    material.startTime = static_cast<int>(startTimeSec * 1000);
    material.endTime = static_cast<int>(endTimeSec * 1000);
    material.duration = material.endTime - material.startTime;
    auto resourse = Resources<Material>::getInstance()->getResource(id);
    material.name = resourse.info->name;
    material.path = resourse.info->path;

    material.command = QString().append("drawtext=");
    material.command.append("fontfile=")
        .append("\\\'") // ' -> \'
        .append(fontFile.replace("\\", "/"))
        .append("\\\'");
    material.command.append(":");
    if (material.path.isEmpty()) {
        material.command.append("text=").append("\'").append(resourse.info->content).append("\'");
    } else {
        material.command.append("textfile=")
            .append("\\\'") // ' -> \'
            .append(material.path.replace("\\", "/"))
            .append("\\\'");
    }
    material.command.append(":");
    material.command.append("x=").append(x);
    material.command.append(":");
    material.command.append("y=").append(y);
    material.command.append(":");
    material.command.append("fontsize=").append(QString::number(fontSize));
    material.command.append(":");
    material.command.append("fontcolor=").append(fontColor);
    material.command.append(":");
    material.command.append("enable=\'between(t,")
        .append(QString::number(startTimeSec))
        .append(",")
        .append(QString::number(endTimeSec))
        .append(")\'");
    m_materialList.push_back(material);
    m_isResVideoProduced = false;
}

void ResVideo::addImage(int id, QString x, QString y, double startTimeSec, double endTimeSec)
{
    MaterialInUse material;
    material.id = id;
    material.type = ResourceType::IMAGE;
    material.startTime = static_cast<int>(startTimeSec * 1000);
    material.endTime = static_cast<int>(endTimeSec * 1000);
    material.duration = material.endTime - material.startTime;
    auto resourse = Resources<Material>::getInstance()->getResource(id);
    material.name = resourse.info->name;
    material.path = resourse.info->path;
    material.command = QString().append("overlay=");
    material.command.append("x=").append(x).append(":").append("y=").append(y);
    material.command.append(":");
    material.command.append("enable=\'between(t,")
        .append(QString::number(startTimeSec))
        .append(",")
        .append(QString::number(endTimeSec))
        .append(")\'");
    m_materialList.push_back(material);
    m_isResVideoProduced = false;
}

void ResVideo::removeVideo(int index)
{
    m_duration -= m_videoList[index].info->duration;
    m_videoList.removeAt(index);
    m_isResVideoProduced = false;
}

void ResVideo::removeVideos(int index, int count)
{
    for (int i = 0; i < count; i++) {
        m_duration -= m_videoList[index].info->duration;
        m_videoList.removeAt(index);
    }
    m_isResVideoProduced = false;
}

void ResVideo::permuteVideo(int oldIndex, int newIndex)
{
    m_videoList.move(oldIndex, newIndex);
    m_isResVideoProduced = false;
}
