#pragma once
#include <QObject>

class DialogManager : public QObject
{
    Q_OBJECT

public:
    DialogManager(QObject *parent = nullptr);
    ~DialogManager();

public slots:
    void onFileImportVideo();
    void onFileImportMaterial();
    void onFileExportVideo();
    void onEditCut(int id);
    void onEditRename(int id);
    void onTimelineAddMaterial(int id);
    void onTimelineDivideVideo();
    void onTimelineMergeVideos();
    void onTimelinePermuteVideos();
};
