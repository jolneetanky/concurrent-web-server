#include "tcp-server/single-thread-server.h"
#include "tcp-server/multi-thread-server.h"
#include "tcp-server/job-queue.h"
#include "thread-pool/thread-pool.h"
#include "types/task.h"

int main()
{
    JobQueue<Task> jobQueue(100);       // shared DS between producer and consumer
    MultiThreadServer server(jobQueue); // producer
    ThreadPool threadPool(jobQueue, 10);

    threadPool.acceptTasks();
    server.start();
}