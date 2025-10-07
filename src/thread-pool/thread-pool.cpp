#include "thread-pool.h"
#include <thread>
#include <chrono>
#include <iostream>

#include <fstream> // for std::ifstream
#include <sstream> // for std::stringstream

#include <unistd.h>     // for read(), write(), close()
#include <sys/socket.h> // for send(), recv(), socket(), bind(), accept()

ThreadPool::ThreadPool(JobQueue<Task> &jq, int size) : m_jobQueue{jq}, m_size{size} {};
void ThreadPool::acceptTasks()
{
    for (int i = 0; i < m_size; i++)
    {
        m_threads.emplace_back([this, i]()
                               { this->_consume(i); });
    }
};

/*
=== PRIVATE HELPER FUNCTIONS ===
*/

// consumer function to take from thread pool
void ThreadPool::_consume(int id)
{
    while (true)
    {
        try
        {
            Task t = m_jobQueue.pop(); // call move ctor; if empty it will throw exception
            std::cout << "Thread " << std::to_string(id) << " popped a task\n";

            int new_socket = t.socket;

            // 5. Read data
            char buffer[1027] = {0}; // C-style character array
            read(new_socket, buffer, 1024);

            // 6. Parse HTTP request

            // 7. Build response
            std::string response = _buildResponse();

            // 8. Send response
            send(new_socket, response.c_str(), response.size(), 0);

            // 9. Close socket
            close(new_socket);
        }
        catch (const JobQueueEmptyException &)
        {
            // std::cout << "Thread " << std::to_string(id) << " failed to pop, going to sleep\n";
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        catch (...)
        {
            std::cout << "Thread " << std::to_string(id) << " failed to pop; an unknown exception occurred\n";
        }
    }
}

std::string ThreadPool::_buildResponse()
{
    // Read file into string
    std::ifstream file("big.html");
    std::stringstream bufferStream;
    bufferStream << file.rdbuf();
    std::string html = bufferStream.str();

    std::string response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: " +
        std::to_string(html.size()) + "\r\n"
                                      "Connection: close\r\n"
                                      "\r\n" +
        html;

    return response;
};