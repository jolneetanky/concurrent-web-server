#include "../src/tcp-server/job-queue.h"
#include <gtest/gtest.h>

// queue of size 3
// push, push, push, push, pop, pop, pop, pop
// expect OK, OK, OK, ERROR, item 1, 2, 3, ERROR

// queue of size 1
// push, push, pop, pop
// expect OK, ERROR, item 1, ERROR

// === Scenario 1: queue of size 3 ===
// push, push, push, push, pop, pop, pop, pop
// expect OK, OK, OK, ERROR, item 1, 2, 3, ERROR
TEST(JobQueueTest, Size3_FullSequence)
{
    JobQueue<int> q(3);

    // Push 3 OK
    EXPECT_NO_THROW(q.push(1));
    EXPECT_NO_THROW(q.push(2));
    EXPECT_NO_THROW(q.push(3));

    // 4th push should throw
    EXPECT_THROW(q.push(4), JobQueueFullException);

    // Pops return items in FIFO order
    EXPECT_EQ(q.pop(), 1);
    EXPECT_EQ(q.pop(), 2);
    EXPECT_EQ(q.pop(), 3);

    // 4th pop should throw
    EXPECT_THROW(q.pop(), JobQueueEmptyException);
}

// === Scenario 2: queue of size 1 ===
// push, push, pop, pop
// expect OK, ERROR, item 1, ERROR
TEST(JobQueueTest, Size1_FullSequence)
{
    JobQueue<int> q(1);

    // First push OK
    EXPECT_NO_THROW(q.push(1));

    // Second push should throw
    EXPECT_THROW(q.push(2), JobQueueFullException);

    // Pop returns the item
    EXPECT_EQ(q.pop(), 1);

    // Next pop throws (empty)
    EXPECT_THROW(q.pop(), JobQueueEmptyException);
}

// 1. Push when not full → size increases by one
TEST(JobQueueTest, PushIncreasesSize)
{
    JobQueue<int> q(2); // capacity = 2
    EXPECT_EQ(q.size(), 0);

    q.push(42);
    EXPECT_EQ(q.size(), 1);

    q.push(99);
    EXPECT_EQ(q.size(), 2);
}

// 2. Push when full → throws overflow_error
TEST(JobQueueTest, PushThrowsWhenFull)
{
    JobQueue<int> q(1); // capacity = 1
    q.push(5);
    EXPECT_THROW(q.push(10), JobQueueFullException);
}

// 3. Pop decreases size
TEST(JobQueueTest, PopDecreasesSize)
{
    JobQueue<int> q(2); // capacity = 2

    q.push(42);

    q.push(99);

    q.pop();
    EXPECT_EQ(q.size(), 1);

    q.pop();
    EXPECT_EQ(q.size(), 0);
}

// 4. Pop throws when empty
TEST(JobQueueTest, PopThrowsWhenEmpty)
{
    JobQueue<int> q(1); // capacity = 1
    EXPECT_THROW(q.pop(), JobQueueEmptyException);
}