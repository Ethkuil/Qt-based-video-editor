#pragma once

class Editor
{
public:
    static Editor *getInstance()
    {
        static Editor *instance = new Editor();
        return instance;
    }
    ~Editor() {}

    void cutVideo(int id, double startTimeS, double endTimeS);
    void divideVideo(int index, double timeS);
    void mergeVideos(int index1); // merge video at index1 and video at index1 + 1

private:
    Editor() {}
};
