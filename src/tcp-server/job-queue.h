#pragma once
#include "../types/task.h"

template <typename T>
class JobQueue
{
public:
    // LATER: think of error endling method
    // TODO: close the socket later
    JobQueue() = default;
    void push(T task) {

    };

    T pop()
    {
        return T();
    };

    // push and pop
    ~JobQueue() = default;
};