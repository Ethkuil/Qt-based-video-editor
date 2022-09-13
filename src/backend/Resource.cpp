#include "Resource.h"

#include <QDebug>
#include <QFile>
#include <QProcess>
#include <QStringList>

Video::Video(QString path)
{
    qDebug() << "Video::Video()" << path;

    this->path = path;
    this->name = path.split("/").last();
    QProcess process;
    process.start("ffprobe.exe",
                  QStringList() << "-show_entries"
                                << "format=duration,size"
                                << "-v"
                                << "quiet"
                                << "-of"
                                << "default=noprint_wrappers=1:nokey=1" << path);
    if (!process.waitForStarted()) {
        qDebug() << "error:" << process.errorString();
        return;
    }
    process.waitForFinished();
    QString output = process.readAllStandardOutput();

    qDebug() << output;

    QStringList outputList = output.split("\r\n");
    // unit: s. Convert to ms
    this->duration = static_cast<int>(outputList[0].toDouble() * 1000);
    // unit: byte. Convert to MB
    this->size = QString("%1MB").arg(outputList[1].toInt() / 1024 / 1024);
}

Material::Material(QString path, ResourceType type)
{
    if (type == ResourceType::TEXT || type == ResourceType::SUBTITLE) {
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "error:" << file.errorString();
            return;
        }
        // read begining part of the file
        this->content = file.read(64);
        if (file.size() > 64) {
            this->content.append("...");
        }
        file.close();
    }
    this->path = path;
    this->name = path.split("/").last();
}

Material::Material(QString name, QString content)
{
    this->name = name;
    this->content = content;
}