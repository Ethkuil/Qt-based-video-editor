#pragma once

/**
 * @brief All the resources currently loaded.
 */

#include <QVector>

#include <QtGlobal>

#include "Resource.h"

template<typename T>
class Resources
{
public:
    ~Resources() { clear(); }

    static Resources *getInstance()
    {
        static Resources<T> *instance = new Resources<T>();
        return instance;
    }

    int getNextId() const { return m_nextId; }

    const Resource<T> &getResource(int id) const
    {
        for (int i = 0; i < m_resources.size(); ++i) {
            if (m_resources[i].id == id) {
                return m_resources[i];
            }
        }
        Q_UNREACHABLE();
    }

    const QVector<Resource<T>> &getResources() const { return m_resources; }

    const Resource<T> &addResource(ResourceType type, T *info)
    {
        Resource<T> resource;
        resource.type = type;
        resource.id = m_nextId++;
        resource.info = info;
        m_resources.push_back(resource);
        return m_resources.back();
    }

    void removeResource(int id)
    {
        for (int i = 0; i < m_resources.size(); ++i) {
            if (m_resources[i].id == id) {
                delete m_resources[i].info;
                m_resources.erase(m_resources.begin() + i);
                return;
            }
        }
        Q_UNREACHABLE();
    }

    void removeResources(const QVector<int> &ids)
    {
        for (int id : ids) {
            removeResource(id);
        }
    }

    void clear() { m_resources.clear(); }

private:
    Resources() {}

    QVector<Resource<T>> m_resources;
    int m_nextId = 0;
};
