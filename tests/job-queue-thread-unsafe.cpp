#include "../src/tcp-server/job-queue.h"
#include <thread>
#include <iostream>
#include <vector>

// This serves to test the thread safety of JobQueue.
int main()
{
    JobQueue<int> q(1000000);

    const int numThreads = 4;
    const int numOps = 100000;

    std::vector<std::thread> threads;

    // Have all 4 threads push 100000 items
    // expected at the of thread, size is 400000
    for (int t = 0; t < numThreads; t++)
    {
        auto worker = [&q, t, numOps]()
        {
            for (int i = 0; i < numOps; i++)
            {
                q.push(i + numOps * t);
                // q.pop();
            }
        };

        threads.emplace_back(worker);
    }

    for (auto &th : threads)
    {
        th.join();
    }

    std::cout << "Queue size: " << std::to_string(q.size()) << "\n";
    return 0;
}
