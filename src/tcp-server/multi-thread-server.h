
#pragma once
#include "tcp-server.h"
#include "job-queue.h"
#include "../types/task.h"

class MultiThreadServer : public TCPServer
{
public:
    MultiThreadServer(JobQueue<Task> &jobQueue);
    void startServer() override;
    ~MultiThreadServer() = default;

private:
    JobQueue<Task> &m_jobQueue;
};