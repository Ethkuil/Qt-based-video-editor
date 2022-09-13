#include "QtBasedVideoEditor.h"

#include <QDebug>
#include <QDialog>
#include <QDir>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>

#include "backend/ResVideo.h"
#include "backend/Resources.h"

QtBasedVideoEditor::QtBasedVideoEditor(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui_QtBasedVideoEditor)
{
    ui->setupUi(this);
    m_dialogManager = new DialogManager(this);
    m_videoListManager = new VideoListManager(ui->videoList, this);
    m_materialLibraryManager = new MaterialLibraryManager(ui->materialList, this);
    m_timelineManager = new TimelineManager(ui->timelineZone, this);
    m_previewWindowManager = new PreviewWindow(ui, this);

    m_videoResources = Resources<Video>::getInstance();
    m_materialResources = Resources<Material>::getInstance();

    connectMenu();
}

QtBasedVideoEditor::~QtBasedVideoEditor()
{
    delete ui;
}

void QtBasedVideoEditor::connectMenu()
{
    // File
    connect(ui->actionImportVideo, &QAction::triggered, [this]() {
        m_dialogManager->onFileImportVideo();
        m_videoListManager->updateVideoList();
    });
    connect(ui->actionExport, &QAction::triggered, [this]() {
        m_dialogManager->onFileExportVideo();
    });
    connect(ui->actionImportMaterial, &QAction::triggered, [this]() {
        m_dialogManager->onFileImportMaterial();
        m_materialLibraryManager->updateMaterialLibrary();
    });

    // Edit
    connect(ui->actionRemoveVideo, &QAction::triggered, [this]() {
        m_videoResources->removeResources(m_videoListManager->getSelectedVideoIds());
        m_videoListManager->updateVideoList();
    });
    connect(ui->actionRemoveMaterial, &QAction::triggered, [this]() {
        m_materialResources->removeResources(m_materialLibraryManager->getSelectedMaterialIds());
        m_materialLibraryManager->updateMaterialLibrary();
    });
    connect(ui->actionCutVideo, &QAction::triggered, [this]() {
        auto ids = m_videoListManager->getSelectedVideoIds();
        for (auto id : ids) {
            m_dialogManager->onEditCut(id);
        }
        m_videoListManager->updateVideoList();
    });
    connect(ui->actionRenameVideo, &QAction::triggered, [this]() {
        auto ids = m_videoListManager->getSelectedVideoIds();
        for (auto id : ids) {
            m_dialogManager->onEditRename(id);
        }
        m_videoListManager->updateVideoList();
    });

    // Timeline
    connect(ui->actionAddVideoToTimeline, &QAction::triggered, [this]() {
        auto ids = m_videoListManager->getSelectedVideoIds();
        for (auto id : ids) {
            ResVideo::getInstance()->addVideo(id);
        }
        m_timelineManager->updateTimeline();
    });
    connect(ui->actionAddMaterialToTimeline, &QAction::triggered, [this]() {
        auto ids = m_materialLibraryManager->getSelectedMaterialIds();
        for (auto id : ids) {
            m_dialogManager->onTimelineAddMaterial(id);
        }
        m_timelineManager->updateTimeline();
    });
    connect(ui->actionDivideVideo, &QAction::triggered, [this]() {
        m_dialogManager->onTimelineDivideVideo();
        m_videoListManager->updateVideoList();
        m_timelineManager->updateTimeline();
    });
    connect(ui->actionMergeVideos, &QAction::triggered, [this]() {
        m_dialogManager->onTimelineMergeVideos();
        m_videoListManager->updateVideoList();
        m_timelineManager->updateTimeline();
    });
    connect(ui->actionPermuteVideosInTimeline, &QAction::triggered, [this]() {
        m_dialogManager->onTimelinePermuteVideos();
        m_timelineManager->updateTimeline();
    });

    // View
    connect(ui->actionPreview, &QAction::triggered, [this]() {
        QString path = m_videoListManager->getFirstSelectedVideoPath();
        qDebug() << "Preview video: " << path;
        m_previewWindowManager->onLoadVideo(path);
    });
    connect(ui->actionPreviewResult, &QAction::triggered, [this]() {
        QString path = ResVideo::getInstance()->produceResVideo();
        qDebug() << "Preview video: " << path;
        m_previewWindowManager->onLoadVideo(path);
    });
}

void QtBasedVideoEditor::closeEvent(QCloseEvent *event)
{
    switch (QMessageBox::information(this,
                                     tr("QtBasedVideoEditor"),
                                     tr("Do you really want to quit?"),
                                     tr("Yes"),
                                     tr("No"),
                                     0,
                                     1)) {
    case 0: {
        event->accept();
        qDebug() << "Delete tmp files";
        QDir dir("./tmp");
        if (dir.exists()) {
            dir.removeRecursively();
        }
        break;
    }
    case 1:
    default:
        event->ignore();
        break;
    }
}
