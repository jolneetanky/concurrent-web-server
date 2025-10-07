
#pragma once
#include "tcp-server.h"
#include "job-queue.h"
#include "../types/task.h"
#include <thread>
#include <vector>
#include <netinet/in.h>

class MultiThreadServer : public TCPServer
{
public:
    MultiThreadServer(JobQueue<Task> &jobQueue);
    void start() override;
    ~MultiThreadServer() = default;

private:
    JobQueue<Task> &m_jobQueue;
    std::vector<std::thread> m_threads;
    int m_server_fd;              // FD representing server socket
    struct sockaddr_in m_address; // holds server address
    int m_addrlen;

    // PRIVATE METHODS
    void _acceptConnections(int id);
};