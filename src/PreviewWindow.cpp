#include "PreviewWindow.h"

#include <QMediaPlayer>
#include <QTimer>
#include <QUrl>
#include <QVideoWidget>

PreviewWindow::PreviewWindow(Ui_QtBasedVideoEditor *ui, QObject *parent)
    : QObject(parent)
    , m_ui(ui)
    , m_isPlaying(false)
    , m_player(nullptr)
{
    m_ui->playZone->setLayout(new QHBoxLayout());
    m_videoWidget = new QVideoWidget(m_ui->playZone);
    m_ui->playZone->layout()->addWidget(m_videoWidget);
    m_videoWidget->setAttribute(Qt::WA_DeleteOnClose);
    m_videoWidget->show();

    // progress bar move as video play
    m_timer = new QTimer(this);
    m_timer->setInterval(100);
    connect(m_timer, &QTimer::timeout, [this]() {
        m_ui->horizontalSlider->setValue(m_player->position());
    });
    connect(m_ui->horizontalSlider, &QSlider::sliderMoved, [this](int position) {
        m_timer->stop();
        m_player->setPosition(position);
    });
    connect(m_ui->horizontalSlider, &QSlider::sliderReleased, [this]() { m_timer->start(); });

    connect(ui->playPauseButton, &QPushButton::clicked, this, &PreviewWindow::onPlayPause);

    connect(m_ui->horizontalSlider, &QSlider::valueChanged, [this](int position) {
        m_ui->timelineSlider->setValue(position);
    });
}

PreviewWindow::~PreviewWindow() {}

void PreviewWindow::onLoadVideo(QString path)
{
    qDebug() << "Loading video from: " << path;
    // each time load a new video, create a new player
    // because file may have been changed even if the path is the same
    delete m_player;
    m_player = new QMediaPlayer(this);
    m_player->setVideoOutput(m_videoWidget);
    m_player->setVolume(20);
    m_player->setMedia(QUrl::fromLocalFile(path));
    connect(m_player, &QMediaPlayer::mediaStatusChanged, [this](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::LoadedMedia) {
            qDebug() << "Video duration: " << m_player->duration();

            m_ui->playPauseButton->setEnabled(true);
            m_ui->horizontalSlider->setEnabled(true);
            m_ui->horizontalSlider->setRange(0, m_player->duration());
            m_ui->timelineSlider->setRange(0, m_player->duration());

            m_timer->start();
            m_player->play();
            m_isPlaying = true;
            m_ui->playPauseButton->setText("Pause");
        } else if (status == QMediaPlayer::EndOfMedia) {
            m_timer->stop();
            m_player->stop();
            m_isPlaying = false;
            m_ui->playPauseButton->setText("Play");
        } else if (status == QMediaPlayer::InvalidMedia) {
            qDebug() << "Invalid media";
        }
    });
}

void PreviewWindow::onPlayPause()
{
    if (m_isPlaying) {
        m_player->pause();
        m_ui->playPauseButton->setText("Play");
        m_timer->stop();
    } else {
        m_player->play();
        m_ui->playPauseButton->setText("Pause");
        m_timer->start();
    }
    m_isPlaying = !m_isPlaying;
}

void PreviewWindow::setPosition(int position)
{
    m_player->setPosition(position);
}
