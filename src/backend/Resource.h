#pragma once
#include <QString>

enum class ResourceType { VIDEO, SUBTITLE, TEXT, IMAGE };

struct Video
{
    QString name;
    int duration; // in ms
    QString size;
    QString path;

    Video(QString path);
};

struct Material
{
    QString name;
    QString content;
    QString path; // empty if not from file

    Material(QString path, ResourceType type); // import from file
    Material(QString name, QString content);   // for text added by typing
};

template<typename T>
struct Resource
{
    ResourceType type;
    int id;
    T *info;
};
