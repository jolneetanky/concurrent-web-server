#include "tcp-server/single-thread-server.h"
#include "tcp-server/multi-thread-server.h"
#include "tcp-server/job-queue.h"
#include "thread-pool/thread-pool.h"
#include "types/task.h"

int main()
{
    JobQueue<Task> jobQueue(5);            // shared DS between producer and consumer
    MultiThreadServer server(jobQueue, 2); // producer
    ThreadPool threadPool(jobQueue, 3);

    // thread pool that subscribes to the same job queue
    threadPool.acceptTasks(); // spawns worker threads to consume from queue. Function immediately returns and is not blocking.
    server.start();           // main thread; blocking

    // SingleThreadServer stServer;
    // stServer.start();
}