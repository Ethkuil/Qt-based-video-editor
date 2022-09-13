#include "Editor.h"

#include <QDebug>
#include <QDir>
#include <QProcess>
#include <QString>

#include "ResVideo.h"
#include "Resources.h"

void Editor::cutVideo(int id, double startTimeS, double endTimeS)
{
    auto video = Resources<Video>::getInstance()->getResource(id);
    QString inputPath = video.info->path;

    QString fileName = inputPath.mid(inputPath.lastIndexOf('/') + 1);
    QString fileNameWithoutSuffix = fileName.mid(0, fileName.lastIndexOf('.'));
    QString fileExt = inputPath.mid(inputPath.lastIndexOf('.'));
    QDir dir("tmp");
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    QString outputPath = "./tmp/" + fileNameWithoutSuffix + QString::number(video.id) + "_cut_"
                         + QString::number(startTimeS) + "_" + QString::number(endTimeS) + fileExt;

    QProcess process;
    process.start("ffmpeg",
                  QStringList() << "-i" << inputPath << "-ss" << QString::number(startTimeS)
                                << "-to" << QString::number(endTimeS) << outputPath);
    if (!process.waitForStarted()) {
        qDebug() << "error:" << process.errorString();
        return;
    }
    process.waitForFinished();
    Resources<Video>::getInstance()->addResource(ResourceType::VIDEO, new Video(outputPath));
}

void Editor::divideVideo(int index, double timeS)
{
    auto video = ResVideo::getInstance()->getVideoList()[index];
    QString inputPath = video.info->path;

    QString fileName = inputPath.mid(inputPath.lastIndexOf('/') + 1);
    QString fileNameWithoutSuffix = fileName.mid(0, fileName.lastIndexOf('.'));
    QString fileExt = inputPath.mid(inputPath.lastIndexOf('.'));
    QDir dir("tmp");
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    QString outputPath1 = "./tmp/" + fileNameWithoutSuffix + QString::number(video.id) + "_cut_"
                          + QString::number(0) + "_" + QString::number(timeS) + fileExt;
    QString outputPath2 = "./tmp/" + fileNameWithoutSuffix + QString::number(video.id) + "_cut_"
                          + QString::number(timeS) + "_"
                          + QString::number(video.info->duration / 1000) + fileExt;

    QProcess process;
    process.start("ffmpeg",
                  QStringList() << "-i" << inputPath << "-ss" << QString::number(0) << "-to"
                                << QString::number(timeS) << "-acodec"
                                << "copy" << outputPath1 << "-ss" << QString::number(timeS) << "-to"
                                << QString::number(video.info->duration / 1000) << "-acodec"
                                << "copy" << outputPath2);
    if (!process.waitForStarted()) {
        qDebug() << "error:" << process.errorString();
        return;
    }
    process.waitForFinished();

    int id1 = Resources<Video>::getInstance()
                  ->addResource(ResourceType::VIDEO, new Video(outputPath1))
                  .id;
    int id2 = Resources<Video>::getInstance()
                  ->addResource(ResourceType::VIDEO, new Video(outputPath2))
                  .id;
    ResVideo::getInstance()->removeVideo(index);
    ResVideo::getInstance()->addVideo(id1, index);
    ResVideo::getInstance()->addVideo(id2, index + 1);
}

void Editor::mergeVideos(int index1)
{
    auto video1 = ResVideo::getInstance()->getVideoList()[index1];
    auto video2 = ResVideo::getInstance()->getVideoList()[index1 + 1];
    QString inputPath1 = video1.info->path;
    QString inputPath2 = video2.info->path;

    QString fileName1 = inputPath1.mid(inputPath1.lastIndexOf('/') + 1);
    QString fileName2 = inputPath2.mid(inputPath2.lastIndexOf('/') + 1);
    QString fileNameWithoutSuffix1 = fileName1.mid(0, fileName1.lastIndexOf('.'));
    QString fileNameWithoutSuffix2 = fileName2.mid(0, fileName2.lastIndexOf('.'));
    QString fileExt1 = inputPath1.mid(inputPath1.lastIndexOf('.'));
    QDir dir("tmp");
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    QString outputPath = "./tmp/merge_" + fileNameWithoutSuffix1 + QString::number(video1.id) + "_"
                         + fileNameWithoutSuffix2 + QString::number(video2.id) + fileExt1;

    QProcess process;
    process.start("ffmpeg",
                  QStringList() << "-i" << inputPath1 << "-i" << inputPath2 << "-filter_complex"
                                << "[0:v][0:a][1:v][1:a]concat=n=2:v=1:a=1[v][a]"
                                << "-map"
                                << "[v]"
                                << "-map"
                                << "[a]" << outputPath);
    if (!process.waitForStarted()) {
        qDebug() << "error:" << process.errorString();
        return;
    }
    process.waitForFinished();

    int id = Resources<Video>::getInstance()
                 ->addResource(ResourceType::VIDEO, new Video(outputPath))
                 .id;
    ResVideo::getInstance()->removeVideo(index1);
    ResVideo::getInstance()->removeVideo(index1);
    ResVideo::getInstance()->addVideo(id, index1);
}
