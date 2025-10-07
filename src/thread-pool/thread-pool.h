#pragma once
#include "../tcp-server/job-queue.h"
#include "../types/task.h"
#include <vector>
#include <thread>

class ThreadPool
{
public:
    ThreadPool(JobQueue<Task> &jq, int size);
    void acceptTasks();

private:
    JobQueue<Task> &m_jobQueue;
    int m_size;
    std::vector<std::thread> m_threads;

    void _consume(int i);
    std::string _buildResponse();
};