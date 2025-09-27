#include "multi-thread-server.h"
#include <netinet/in.h>
#include <iostream>
#include <unistd.h> // for close()

#define PORT 8080

MultiThreadServer::MultiThreadServer(JobQueue<Task> &jobQueue) : m_jobQueue{jobQueue} {};

void MultiThreadServer::startServer()
{
    // 1) open connections
    // 2) listen for requests
    // 3) for each request that comes in, open a new connection, and place that connection in a shared queue
    // 4) notify worker threads, who will pick it up when notified
    // 5) worker thread handles request and sends response

    // POTENTIAL CONS OR WTV:
    // depending on how the queue is implemented, this cuold become sequential,
    // especially because every thread will modify the queue.
    // one solution is to have sharded queue - each worker manages one queue shard -> minimize cross-core contention.
    // that also allows us to have lock-free queue.

    // server_fd: server file descriptor
    // main thread listens and pushes to shared queue
    int server_fd, new_socket;
    struct sockaddr_in address; // IP address of host machine
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1027] = {0}; // C-style character array

    // 1. Create socket, using sock_stream (TCP)
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // 2. Bind socket to a local address + port number
    // Set socket options
    // SOL_SOCKET = socket-level options
    // SO_REUSE_ADDR = allow reuse of a local address/port
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    address.sin_family = AF_INET;         // IP address family (IPv4, IPv6, etc.)
    address.sin_addr.s_addr = INADDR_ANY; // Allow incoming requests from any network medium (eg. wifi, public IP, etc.) route them to this server.
    address.sin_port = htons(PORT);

    // Bind
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // 3. Listen for incoming requests (blocks until connection received)
    // listen(sockfd, i) => `i` is the max number of pending connections that can be queued while server is busy.
    std::cout << "Server up and running" << "\n";

    // no need to test this I guess because it's alr a standard lib
    // sequential server - this thread listens for one connection, resopnds to it, then closes that connection
    while (true)
    {
        if (listen(server_fd, 3) < 0)
        {
            perror("listen failed");
            exit(EXIT_FAILURE);
        }

        // 4. Accept incoming connection, create a new socket for that connection
        // Open up another connection
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        if (new_socket < 0)
        {
            perror("failed to accept");
            exit(EXIT_FAILURE);
        }

        // ADD TO TO SHARED QUEUE
        // JobQueue.submit(new_socket);
        // worker responsible for closing the socket
        // m_jobQueue.submit(Task{std::move(new_socket)});

        /*
        // 5. Read data
        read(new_socket, buffer, 1024);
        std::cout << "Message from client: " << buffer << std::endl;

        // 6. Parse HTTP request

        // 7. Build response
        std::string response = buildResponse();

        // 8. Send response
        send(new_socket, response.c_str(), response.size(), 0);

        // Close socket
        */
        close(new_socket);
    }
    close(server_fd);
}