#include "tcp-server/single-thread-server.h"
#include "tcp-server/multi-thread-server.h"
#include "tcp-server/job-queue.h"
#include "types/task.h"

int main()
{
    // SingleThreadServer server;
    // server.startServer();

    JobQueue<Task> jobQueue;
    MultiThreadServer server(jobQueue);
    server.startServer();
}