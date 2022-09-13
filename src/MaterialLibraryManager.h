#pragma once
#include <QListWidget>
#include <QObject>
#include <QStringList>
#include <QVector>

#include "backend/Resource.h"

class MaterialLibraryManager : public QObject
{
    Q_OBJECT

public:
    MaterialLibraryManager(QListWidget *materialLibrary, QObject *parent = nullptr);
    ~MaterialLibraryManager();

    int getFirstSelectedMaterialId();
    const Resource<Material> *getFirstSelectedMaterialResource();

    QVector<int> getSelectedMaterialIds();
    const QVector<const Resource<Material> *> getSelectedMaterialResources();

public slots:
    void updateMaterialLibrary();

private:
    QListWidget *m_materialLibrary;

    void appendMaterial(const Resource<Material> &resource);
    void clearMaterialLibrary();
};