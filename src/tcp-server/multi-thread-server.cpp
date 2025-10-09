#include "multi-thread-server.h"
#include <iostream>
#include <unistd.h> // for close()

#define PORT 8080

MultiThreadServer::MultiThreadServer(JobQueue<Task> &jobQueue) : m_jobQueue{jobQueue} {};

void MultiThreadServer::start()
{
    int opt = 1;
    m_addrlen = sizeof(m_address);
    char buffer[1027] = {0}; // C-style character array

    // 1. Create socket, using sock_stream (TCP)
    m_server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_server_fd == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // 2. Bind socket to a local address + port number
    // Set socket options
    // SOL_SOCKET = socket-level options
    // SO_REUSE_ADDR = allow reuse of a local address/port
    setsockopt(m_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    m_address.sin_family = AF_INET;         // IP address family (IPv4, IPv6, etc.)
    m_address.sin_addr.s_addr = INADDR_ANY; // Allow incoming requests from any network medium (eg. wifi, public IP, etc.) route them to this server.
    m_address.sin_port = htons(PORT);

    // Bind socket to fixed port
    if (bind(m_server_fd, (struct sockaddr *)&m_address, sizeof(m_address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // listen - mark the socket (`m_server_fd`) as a server socket, that listens for incoming connection requests
    // 3 is the backlog, ie. how many pending connections OS can queue while server is busy
    // these connections are added to a back of a queue managed by OS.
    // when we `accept`, we dequeue one of these connections.
    if (listen(m_server_fd, 3) < 0)
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server up and running" << "\n";

    // listen for connections, accpet them, and push to job queue
    while (true)
    {
        // 4. Accept incoming connection, create a new socket for that connection. `accept` is blocking.
        int new_socket = accept(m_server_fd, (struct sockaddr *)&m_address, (socklen_t *)&m_addrlen);
        if (new_socket < 0)
        {
            std::cerr << "Failed to accept: " << std::strerror(errno) << std::endl;
            exit(EXIT_FAILURE);
        }

        m_jobQueue.wait_and_push(Task(new_socket));
    }
}