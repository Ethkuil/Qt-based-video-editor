#include "TimelineManager.h"

#include <QDebug>
#include <QList>
#include <QListWidget>
#include <QSize>
#include <QVBoxLayout>
#include <QtGlobal>

#include "backend/ResVideo.h"

TimelineManager::TimelineManager(QWidget *timelineZone, QObject *parent)
    : m_timelineZone(timelineZone)
    , QObject(parent)
{
    m_colorPool << QColor("red") << QColor("green") << QColor("blue") << QColor("yellow")
                << QColor("purple") << QColor("cyan") << QColor("orange") << QColor("pink")
                << QColor("gray");
}

TimelineManager::~TimelineManager() {}

void TimelineManager::setItem(QListWidgetItem *item, int id, QString name)
{
    item->setBackgroundColor(m_colorPool[id % m_colorPool.size()]);
    item->setData(Qt::UserRole, id);
    item->setText(QString::number(id) + ":" + name);
    item->setTextColor(QColor("white"));
    item->setFont(QFont("Microsoft YaHei", 14));
}

QListWidget *createTimeline(int height, QWidget *parent)
{
    auto axis = new QListWidget(parent);
    axis->setFlow(QListWidget::LeftToRight);
    axis->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    axis->setFixedHeight(height);
    axis->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    axis->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    return axis;
}

void TimelineManager::updateTimeline()
{
    // clear timeline
    auto vLayout = m_timelineZone->layout();
    if (vLayout) {
        QLayoutItem *child;
        while ((child = vLayout->takeAt(0)) != nullptr) {
            delete child->widget();
            delete child;
        }
    }

    const int totalWidth = m_timelineZone->width();
    const int height = 100;
    auto resVideo = ResVideo::getInstance();
    const int totalDuration = resVideo->getDuration();

    // video axis
    auto videoAxis = createTimeline(height, m_timelineZone);
    // add video
    for (auto &video : resVideo->getVideoList()) {
        auto item = new QListWidgetItem(videoAxis);
        setItem(item, video.id, video.info->name);
        const int duration = video.info->duration;
        item->setSizeHint(QSize(totalWidth * duration / totalDuration, height));
        videoAxis->addItem(item);
    }
    vLayout->addWidget(videoAxis);

    // material axis without time
    // added to timeline zone finally
    auto materialAxisWithoutTime = createTimeline(height, m_timelineZone);
    materialAxisWithoutTime->setStyleSheet("background-color: rgb(200, 200, 200);"); // gray

    // material axises
    // when space is not enough, a new axis will be created
    QList<QListWidget *> materialAxisList;
    materialAxisList << createTimeline(height, m_timelineZone);
    for (auto &material : resVideo->getMaterialList()) {
        const int duration = material.duration;
        if (duration < 0) {
            // no duration, add to material axis without time
            auto item = new QListWidgetItem(materialAxisWithoutTime);
            setItem(item, material.id, material.name);
            materialAxisWithoutTime->addItem(item);
        } else {
            qDebug() << "material"
                     << "id" << material.id << "duration" << duration << "command"
                     << material.command;
            int width = totalDuration > 0 ? qMin(totalWidth * duration / totalDuration, totalWidth)
                                          : totalWidth;
            // add to axis if space is enough. otherwise, create a new axis
            bool added = false;
            for (auto axis : materialAxisList) {
                int curTotalWidth = 0;
                for (int i = 0; i < axis->count(); i++) {
                    curTotalWidth += axis->item(i)->sizeHint().width();
                }
                qDebug() << "curTotalWidth" << curTotalWidth << "width" << width;
                if (curTotalWidth + width <= totalWidth) {
                    auto item = new QListWidgetItem(axis);
                    setItem(item, material.id, material.name);
                    item->setSizeHint(QSize(width, height));
                    axis->addItem(item);
                    added = true;
                    break;
                }
            }
            if (!added) {
                auto newMaterialAxis = createTimeline(height, m_timelineZone);
                auto item = new QListWidgetItem(newMaterialAxis);
                setItem(item, material.id, material.name);
                item->setSizeHint(QSize(width, height));
                newMaterialAxis->addItem(item);
                materialAxisList << newMaterialAxis;
            }
        }
    }

    vLayout->addWidget(materialAxisWithoutTime);
    for (auto axis : materialAxisList) {
        vLayout->addWidget(axis);
    }
}
