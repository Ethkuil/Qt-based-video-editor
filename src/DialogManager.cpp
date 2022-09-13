#include "DialogManager.h"

#include <QComboBox>
#include <QDialog>
#include <QFileDialog>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QString>

#include "backend/Editor.h"
#include "backend/ResVideo.h"
#include "backend/Resource.h"
#include "backend/Resources.h"

DialogManager::DialogManager(QObject *parent)
    : QObject(parent)
{}

DialogManager::~DialogManager() {}

void DialogManager::onFileImportVideo()
{
    QStringList paths = QFileDialog::getOpenFileNames(nullptr,
                                                      "Import Video",
                                                      "",
                                                      "Video Files (*.mp4 *.mkv *.avi)");
    for (const QString &path : paths) {
        Resources<Video>::getInstance()->addResource(ResourceType::VIDEO, new Video(path));
    }
}

void DialogManager::onFileImportMaterial()
{
    QStringList paths = QFileDialog::getOpenFileNames(
        nullptr,
        "Import Material",
        "",
        "Material Files (*.png *.jpg *.jpeg *.bmp *.txt *ass *.srt *.vtt)");
    for (const QString &path : paths) {
        // recognize file type by extension
        ResourceType type;
        QString suffix = path.split(".").last();
        if (suffix == "png" || suffix == "jpg" || suffix == "jpeg" || suffix == "bmp") {
            type = ResourceType::IMAGE;
        } else if (suffix == "srt" || suffix == "ass" || suffix == "vtt") {
            type = ResourceType::SUBTITLE;
        } else {
            type = ResourceType::TEXT;
        }
        Resources<Material>::getInstance()->addResource(type, new Material(path, type));
    }
}

void DialogManager::onFileExportVideo()
{
    QDialog *dialog = new QDialog();
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setWindowTitle("Export Video");
    dialog->setMinimumSize(400, 200);
    dialog->setLayout(new QGridLayout());

    dialog->layout()->addWidget(new QLabel("Export as:"));
    QComboBox *formatComboBox = new QComboBox(dialog);
    formatComboBox->addItem("mkv");
    formatComboBox->addItem("mp4");
    formatComboBox->addItem("avi");
    formatComboBox->addItem("flv");
    formatComboBox->addItem("mov");
    formatComboBox->addItem("wmv");
    formatComboBox->setCurrentIndex(0);

    dialog->layout()->addWidget(formatComboBox);

    dialog->layout()->addWidget(new QLabel("resolution", dialog));
    QComboBox *resolutionComboBox = new QComboBox(dialog);
    resolutionComboBox->addItem("640x480");   // 480p
    resolutionComboBox->addItem("1280x720");  // 720p
    resolutionComboBox->addItem("1920x1080"); // 1080p
    resolutionComboBox->setCurrentIndex(0);
    dialog->layout()->addWidget(resolutionComboBox);

    dialog->layout()->addWidget(new QLabel("video bitrate", dialog));
    QComboBox *videoBitrateComboBox = new QComboBox(dialog);
    videoBitrateComboBox->addItem("1000k");
    videoBitrateComboBox->addItem("2000k");
    videoBitrateComboBox->addItem("4000k");
    videoBitrateComboBox->setCurrentIndex(0);
    dialog->layout()->addWidget(videoBitrateComboBox);

    QPushButton *exportButton = new QPushButton("Export", dialog);
    dialog->layout()->addWidget(exportButton);
    connect(exportButton,
            &QPushButton::clicked,
            [this, dialog, formatComboBox, resolutionComboBox, videoBitrateComboBox]() {
                QString format = formatComboBox->currentText();
                QString path = QFileDialog::getSaveFileName(dialog,
                                                            "Export Video",
                                                            "",
                                                            "Video Files (*." + format + ")");
                if (!path.isEmpty()) {
                    dialog->close();

                    // export video
                    QString resolution = resolutionComboBox->currentText();
                    QString videoBitrate = videoBitrateComboBox->currentText();
                    ResVideo::getInstance()->exportVideo(resolution.split("x").first().toInt(),
                                                         resolution.split("x").last().toInt(),
                                                         videoBitrate.split("k").first().toInt(),
                                                         path);
                    // export finished
                    QMessageBox::information(nullptr,
                                             "Export Video",
                                             "Export finished",
                                             QMessageBox::Ok);
                }
            });

    dialog->exec();
}

void DialogManager::onEditCut(int id)
{
    // pop up a dialog to get the start and end time
    QDialog *dialog = new QDialog();
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setWindowTitle("Cut Video");
    dialog->setMinimumSize(400, 200);
    dialog->setLayout(new QGridLayout());
    dialog->layout()->addWidget(new QLabel("Start Time(s):", dialog));

    // number input
    QLineEdit *startTimeEdit = new QLineEdit(dialog);
    dialog->layout()->addWidget(startTimeEdit);
    dialog->layout()->addWidget(new QLabel("End Time(s):", dialog));
    QLineEdit *endTimeEdit = new QLineEdit(dialog);
    dialog->layout()->addWidget(endTimeEdit);

    // confirm button
    QPushButton *confirmButton = new QPushButton("Confirm", dialog);
    dialog->layout()->addWidget(confirmButton);

    connect(confirmButton, &QPushButton::clicked, [this, dialog, id, startTimeEdit, endTimeEdit]() {
        dialog->close();
        double startTime = startTimeEdit->text().toDouble();
        double endTime = endTimeEdit->text().toDouble();
        Editor::getInstance()->cutVideo(id, startTime, endTime);
        // cut finished
        QMessageBox::information(nullptr, "Cut Video", "Cut finished", QMessageBox::Ok);
    });

    dialog->exec();
}

void DialogManager::onEditRename(int id)
{
    // pop up a dialog to get the new name
    QDialog *dialog = new QDialog();
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setWindowTitle("Rename");
    dialog->setLayout(new QGridLayout());
    dialog->layout()->addWidget(new QLabel("New Name:", dialog));

    // text input
    QLineEdit *newNameEdit = new QLineEdit(dialog);
    dialog->layout()->addWidget(newNameEdit);

    // confirm button
    QPushButton *confirmButton = new QPushButton("Confirm", dialog);
    dialog->layout()->addWidget(confirmButton);

    connect(confirmButton, &QPushButton::clicked, [this, dialog, id, newNameEdit]() {
        QString newName = newNameEdit->text();
        Resources<Video>::getInstance()->getResource(id).info->name = newName;
        dialog->close();
    });
    dialog->exec();
}

void DialogManager::onTimelineAddMaterial(int id)
{
    auto &material = Resources<Material>::getInstance()->getResource(id);
    switch (material.type) {
    case ResourceType::IMAGE: {
        QDialog *dialog = new QDialog();
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        dialog->setWindowTitle("Add Image");
        dialog->setMinimumSize(400, 200);
        dialog->setLayout(new QGridLayout());
        // QString x, QString y, double startTimeSec, double endTimeSec
        dialog->layout()->addWidget(new QLabel("x(supports ffmpeg grammar, such as:w/2):", dialog));
        QLineEdit *xEdit = new QLineEdit(dialog);
        dialog->layout()->addWidget(xEdit);
        dialog->layout()->addWidget(new QLabel("y(supports ffmpeg grammar, such as:h/2):", dialog));
        QLineEdit *yEdit = new QLineEdit(dialog);
        dialog->layout()->addWidget(yEdit);
        dialog->layout()->addWidget(new QLabel("Start Time(s, decimal is supported):", dialog));
        QLineEdit *startTimeEdit = new QLineEdit(dialog);
        dialog->layout()->addWidget(startTimeEdit);
        dialog->layout()->addWidget(new QLabel("End Time(s, decimal is supported):", dialog));
        QLineEdit *endTimeEdit = new QLineEdit(dialog);
        dialog->layout()->addWidget(endTimeEdit);
        QPushButton *confirmButton = new QPushButton("Confirm", dialog);
        dialog->layout()->addWidget(confirmButton);
        connect(confirmButton,
                &QPushButton::clicked,
                [this, dialog, id, xEdit, yEdit, startTimeEdit, endTimeEdit]() {
                    dialog->close();
                    QString x = xEdit->text();
                    QString y = yEdit->text();
                    double startTime = startTimeEdit->text().toDouble();
                    double endTime = endTimeEdit->text().toDouble();
                    ResVideo::getInstance()->addImage(id, x, y, startTime, endTime);
                });
        dialog->exec();
        break;
    }
    case ResourceType::SUBTITLE: {
        ResVideo::getInstance()->addSubtitle(id);
        break;
    }
    case ResourceType::TEXT: {
        QDialog *dialog = new QDialog();
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        dialog->setWindowTitle("Add Text");
        dialog->setMinimumSize(400, 200);
        dialog->setLayout(new QGridLayout());
        dialog->layout()->addWidget(
            new QLabel("x(support ffmpeg grammar, such as:(w-tw)/2)):", dialog));
        QLineEdit *xEdit = new QLineEdit(dialog);
        dialog->layout()->addWidget(xEdit);
        dialog->layout()->addWidget(
            new QLabel("y(support ffmpeg grammar, such as:(h-th)/2)):", dialog));
        QLineEdit *yEdit = new QLineEdit(dialog);
        dialog->layout()->addWidget(yEdit);
        dialog->layout()->addWidget(new QLabel("Start Time(s, decimal is supported):", dialog));
        QLineEdit *startTimeEdit = new QLineEdit(dialog);
        dialog->layout()->addWidget(startTimeEdit);
        dialog->layout()->addWidget(new QLabel("End Time(s, decimal is supported):", dialog));
        QLineEdit *endTimeEdit = new QLineEdit(dialog);
        dialog->layout()->addWidget(endTimeEdit);
        dialog->layout()->addWidget(new QLabel("Font Size(the default is 30):", dialog));
        QLineEdit *fontSizeEdit = new QLineEdit(dialog);
        fontSizeEdit->setText("30");
        dialog->layout()->addWidget(fontSizeEdit);
        dialog->layout()->addWidget(new QLabel(
            "Font Color(The default is white. Supports ffmpeg grammar, such as red, #ff0000):",
            dialog));
        QLineEdit *fontColorEdit = new QLineEdit(dialog);
        fontColorEdit->setText("white");
        dialog->layout()->addWidget(fontColorEdit);
        dialog->layout()->addWidget(
            new QLabel("Font File(The default is ms-yahei, to support more languages):", dialog));
        QLineEdit *fontFileEdit = new QLineEdit(dialog);
        fontFileEdit->setText(ResVideo::getInstance()->getDefaultFontFile());
        dialog->layout()->addWidget(fontFileEdit);
        QPushButton *confirmButton = new QPushButton("Confirm", dialog);
        dialog->layout()->addWidget(confirmButton);
        connect(confirmButton,
                &QPushButton::clicked,
                [this,
                 dialog,
                 id,
                 xEdit,
                 yEdit,
                 startTimeEdit,
                 endTimeEdit,
                 fontSizeEdit,
                 fontColorEdit,
                 fontFileEdit]() {
                    dialog->close();
                    QString x = xEdit->text();
                    QString y = yEdit->text();
                    double startTime = startTimeEdit->text().toDouble();
                    double endTime = endTimeEdit->text().toDouble();
                    int fontSize = fontSizeEdit->text().toInt();
                    QString fontColor = fontColorEdit->text();
                    QString fontFile = fontFileEdit->text();
                    ResVideo::getInstance()
                        ->addText(id, x, y, startTime, endTime, fontSize, fontColor, fontFile);
                });
        dialog->exec();
        break;
    }
    }
}

void DialogManager::onTimelineDivideVideo()
{
    QDialog *dialog = new QDialog();
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setWindowTitle("Divide Video");
    dialog->setMinimumSize(400, 200);
    dialog->setLayout(new QGridLayout());
    dialog->layout()->addWidget(new QLabel("Video Index in Timeline(from 0):", dialog));
    QSpinBox *indexEdit = new QSpinBox(dialog);
    indexEdit->setRange(0, ResVideo::getInstance()->getVideoList().size() - 1);
    dialog->layout()->addWidget(indexEdit);
    dialog->layout()->addWidget(new QLabel("Divide Time(s, decimal is supported):", dialog));
    QLineEdit *timeEdit = new QLineEdit(dialog);
    dialog->layout()->addWidget(timeEdit);
    QPushButton *confirmButton = new QPushButton("Confirm", dialog);
    dialog->layout()->addWidget(confirmButton);
    connect(confirmButton, &QPushButton::clicked, [this, dialog, indexEdit, timeEdit]() {
        dialog->close();
        int index = indexEdit->value();
        double timeS = timeEdit->text().toDouble();
        Editor::getInstance()->divideVideo(index, timeS);
        QMessageBox::information(nullptr, "Divide Video", "Divide Video Success!");
    });
    dialog->exec();
}

void DialogManager::onTimelineMergeVideos()
{
    QDialog *dialog = new QDialog();
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setWindowTitle("Merge Video");
    dialog->setMinimumSize(400, 200);
    dialog->setLayout(new QGridLayout());
    dialog->layout()->addWidget(
        new QLabel("First of 2 Videos to Merge Index in Timeline(from 0):", dialog));
    QSpinBox *indexEdit1 = new QSpinBox(dialog);
    indexEdit1->setRange(0,
                         ResVideo::getInstance()->getVideoList().size()
                             - 2); // after the last video it's nothing to merge, so -2
    dialog->layout()->addWidget(indexEdit1);
    QPushButton *confirmButton = new QPushButton("Confirm", dialog);
    dialog->layout()->addWidget(confirmButton);
    connect(confirmButton, &QPushButton::clicked, [this, dialog, indexEdit1]() {
        dialog->close();
        int index1 = indexEdit1->value();
        Editor::getInstance()->mergeVideos(index1);
        QMessageBox::information(nullptr, "Merge Video", "Merge Video Success!");
    });
    dialog->exec();
}

void DialogManager::onTimelinePermuteVideos()
{
    QDialog *dialog = new QDialog();
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setWindowTitle("Permute Video");
    dialog->setMinimumSize(400, 200);
    dialog->setLayout(new QGridLayout());
    dialog->layout()->addWidget(new QLabel("Video Index in Timeline(from 0):", dialog));
    QSpinBox *indexEdit = new QSpinBox(dialog);
    indexEdit->setRange(0, ResVideo::getInstance()->getVideoList().size() - 1);
    dialog->layout()->addWidget(indexEdit);
    dialog->layout()->addWidget(new QLabel("New Position in Timeline(from 0):", dialog));
    QSpinBox *positionEdit = new QSpinBox(dialog);
    positionEdit->setRange(0, ResVideo::getInstance()->getVideoList().size() - 1);
    dialog->layout()->addWidget(positionEdit);
    QPushButton *confirmButton = new QPushButton("Confirm", dialog);
    dialog->layout()->addWidget(confirmButton);
    connect(confirmButton, &QPushButton::clicked, [this, dialog, indexEdit, positionEdit]() {
        dialog->close();
        int index = indexEdit->value();
        int position = positionEdit->value();
        ResVideo::getInstance()->permuteVideo(index, position);
    });
    dialog->exec();
}
