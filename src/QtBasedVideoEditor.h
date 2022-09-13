#pragma once
#include "ui_QtBasedVideoEditor.h"

#include <QCloseEvent>
#include <QMainWindow>

#include "backend/Resources.h"

#include "DialogManager.h"
#include "MaterialLibraryManager.h"
#include "PreviewWindow.h"
#include "TimelineManager.h"
#include "VideoListManager.h"
class QtBasedVideoEditor : public QMainWindow
{
    Q_OBJECT

public:
    QtBasedVideoEditor(QWidget *parent = nullptr);
    ~QtBasedVideoEditor();

    void closeEvent(QCloseEvent *event) override;

private:
    Ui_QtBasedVideoEditor *ui;
    DialogManager *m_dialogManager;
    VideoListManager *m_videoListManager;
    MaterialLibraryManager *m_materialLibraryManager;
    TimelineManager *m_timelineManager;
    PreviewWindow *m_previewWindowManager;

    Resources<Video> *m_videoResources;
    Resources<Material> *m_materialResources;

    void connectMenu();
    void clearTmp();
};
