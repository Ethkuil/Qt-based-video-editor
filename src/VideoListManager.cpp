#include "VideoListManager.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QWidget>

#include "backend/Resource.h"
#include "backend/Resources.h"

VideoListManager::VideoListManager(QListWidget *videoList, QObject *parent)
    : m_videoListWidget(videoList)
    , QObject(parent)
{
    m_videoListWidget->setSelectionMode(
        QAbstractItemView::ExtendedSelection); // Allow multiple selection

    // header for video list
    QWidget *headerWidget = new QWidget(m_videoListWidget);
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->addWidget(new QLabel("ID"));
    headerLayout->addWidget(new QLabel("Name"));
    headerLayout->addWidget(new QLabel("Duration"));
    headerLayout->addWidget(new QLabel("Size"));
    headerWidget->setLayout(headerLayout);
    QListWidgetItem *headerItem = new QListWidgetItem(m_videoListWidget);
    headerItem->setData(Qt::UserRole, -1);
    headerItem->setSizeHint(headerWidget->sizeHint());
    m_videoListWidget->addItem(headerItem);
    m_videoListWidget->setItemWidget(headerItem, headerWidget);
}

VideoListManager::~VideoListManager() {}

int VideoListManager::getFirstSelectedVideoId()
{
    if (m_videoListWidget->selectedItems().size() > 0) {
        int id = m_videoListWidget->selectedItems().first()->data(Qt::UserRole).toInt();
        if (id != -1) {
            return id;
        }
    }
    return -1;
}

QString VideoListManager::getFirstSelectedVideoPath()
{
    if (m_videoListWidget->selectedItems().size() > 0) {
        int id = m_videoListWidget->selectedItems().first()->data(Qt::UserRole).toInt();
        if (id != -1) {
            return Resources<Video>::getInstance()->getResource(id).info->path;
        }
    }
    return "";
}

const Resource<Video> *VideoListManager::getFirstSelectedVideoResource()
{
    if (m_videoListWidget->selectedItems().size() > 0) {
        int id = m_videoListWidget->selectedItems().first()->data(Qt::UserRole).toInt();
        if (id != -1) {
            return &Resources<Video>::getInstance()->getResource(id);
        }
    }
    return nullptr;
}

QVector<int> VideoListManager::getSelectedVideoIds()
{
    QVector<int> ids;
    for (auto item : m_videoListWidget->selectedItems()) {
        int id = item->data(Qt::UserRole).toInt();
        if (id != -1) {
            ids.push_back(id);
        }
    }
    return ids;
}

const QVector<const Resource<Video> *> VideoListManager::getSelectedVideoResources()
{
    QVector<const Resource<Video> *> resources;
    for (auto item : m_videoListWidget->selectedItems()) {
        int id = item->data(Qt::UserRole).toInt();
        if (id != -1) {
            resources.push_back(&Resources<Video>::getInstance()->getResource(id));
        }
    }
    return resources;
}

void VideoListManager::appendVideo(const Resource<Video> &resource)
{
    QWidget *videoWidget = new QWidget(m_videoListWidget);
    QHBoxLayout *videoItemLayout = new QHBoxLayout(videoWidget);
    videoWidget->setLayout(videoItemLayout);

    Video *info = resource.info;
    qDebug() << "VideoListManager::appendVideo: " << info->path;
    videoItemLayout->addWidget(new QLabel(QString::number(resource.id)));
    videoItemLayout->addWidget(new QLabel(info->name));
    int durationS = info->duration / 1000;
    QString duration = QString("%1:%2:%3")
                           .arg(durationS / 3600, 2, 10, QChar('0'))
                           .arg((durationS % 3600) / 60, 2, 10, QChar('0'))
                           .arg(durationS % 60, 2, 10, QChar('0'));
    videoItemLayout->addWidget(new QLabel(duration));
    videoItemLayout->addWidget(new QLabel(info->size));

    QListWidgetItem *videoItem = new QListWidgetItem();
    videoItem->setSizeHint(videoWidget->sizeHint());
    videoItem->setData(Qt::UserRole, resource.id); // used to identify the video
    m_videoListWidget->addItem(videoItem);
    m_videoListWidget->setItemWidget(videoItem, videoWidget);
}

void VideoListManager::clearVideoList()
{
    // clear all items except the header
    for (int i = m_videoListWidget->count() - 1; i > 0; --i) {
        m_videoListWidget->takeItem(i);
    }
}

void VideoListManager::updateVideoList()
{
    qDebug() << "VideoListManager::updateVideoList";
    clearVideoList();
    for (auto &resource : Resources<Video>::getInstance()->getResources()) {
        appendVideo(resource);
    }
}
