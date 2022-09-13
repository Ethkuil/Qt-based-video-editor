#include "MaterialLibraryManager.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QWidget>

#include "backend/Resource.h"
#include "backend/Resources.h"

MaterialLibraryManager::MaterialLibraryManager(QListWidget *materialLibrary, QObject *parent)
    : QObject(parent)
    , m_materialLibrary(materialLibrary)
{
    m_materialLibrary->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);

    // header for material library
    QWidget *headerWidget = new QWidget(m_materialLibrary);
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->addWidget(new QLabel("ID"));
    headerLayout->addWidget(new QLabel("Name"));
    headerLayout->addWidget(new QLabel("Content"));
    headerWidget->setLayout(headerLayout);
    QListWidgetItem *headerItem = new QListWidgetItem(m_materialLibrary);
    headerItem->setData(Qt::UserRole, -1);
    headerItem->setSizeHint(headerWidget->sizeHint());
    m_materialLibrary->addItem(headerItem);
    m_materialLibrary->setItemWidget(headerItem, headerWidget);
}

MaterialLibraryManager::~MaterialLibraryManager() {}

int MaterialLibraryManager::getFirstSelectedMaterialId()
{
    if (m_materialLibrary->selectedItems().size() > 0) {
        int id = m_materialLibrary->selectedItems().first()->data(Qt::UserRole).toInt();
        if (id != -1) {
            return id;
        }
    }
    return -1;
}

const Resource<Material> *MaterialLibraryManager::getFirstSelectedMaterialResource()
{
    if (m_materialLibrary->selectedItems().size() > 0) {
        int id = m_materialLibrary->selectedItems().first()->data(Qt::UserRole).toInt();
        if (id != -1) {
            return &Resources<Material>::getInstance()->getResource(id);
        }
    }
    return nullptr;
}

QVector<int> MaterialLibraryManager::getSelectedMaterialIds()
{
    QVector<int> ids;
    for (QListWidgetItem *item : m_materialLibrary->selectedItems()) {
        int id = item->data(Qt::UserRole).toInt();
        if (id != -1) {
            ids.push_back(id);
        }
    }
    return ids;
}

const QVector<const Resource<Material> *> MaterialLibraryManager::getSelectedMaterialResources()
{
    QVector<const Resource<Material> *> resources;
    for (QListWidgetItem *item : m_materialLibrary->selectedItems()) {
        int id = item->data(Qt::UserRole).toInt();
        if (id != -1) {
            resources.push_back(&Resources<Material>::getInstance()->getResource(id));
        }
    }
    return resources;
}

void MaterialLibraryManager::appendMaterial(const Resource<Material> &resource)
{
    QWidget *materialWidget = new QWidget(m_materialLibrary);
    QHBoxLayout *materialItemLayout = new QHBoxLayout(materialWidget);
    materialWidget->setLayout(materialItemLayout);

    Material *info = resource.info;
    materialItemLayout->addWidget(new QLabel(QString::number(resource.id)));
    materialItemLayout->addWidget(new QLabel(info->name));
    materialItemLayout->addWidget(new QLabel(info->content));

    QListWidgetItem *materialItem = new QListWidgetItem();
    materialItem->setSizeHint(materialWidget->sizeHint());
    materialItem->setData(Qt::UserRole, resource.id); // used to identify the material
    m_materialLibrary->addItem(materialItem);
    m_materialLibrary->setItemWidget(materialItem, materialWidget);
}

void MaterialLibraryManager::clearMaterialLibrary()
{
    // clear all items except header
    for (int i = m_materialLibrary->count() - 1; i > 0; i--) {
        QListWidgetItem *item = m_materialLibrary->takeItem(i);
        delete item;
    }
}

void MaterialLibraryManager::updateMaterialLibrary()
{
    clearMaterialLibrary();
    for (auto resource : Resources<Material>::getInstance()->getResources()) {
        appendMaterial(resource);
    }
}
