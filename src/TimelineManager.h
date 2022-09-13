#pragma once

#include <QColor>
#include <QList>
#include <QListWidgetItem>
#include <QObject>
#include <QWidget>

/**
 * @brief Display the timeline.
 */

class TimelineManager : public QObject
{
    Q_OBJECT

public:
    TimelineManager(QWidget *timelineZone, QObject *parent = nullptr);
    ~TimelineManager();

public slots:
    void updateTimeline();

private:
    QWidget *m_timelineZone;
    QList<QColor> m_colorPool;

    void setItem(QListWidgetItem *item, int id, QString name);
};
