#pragma once
#include "../types/task.h"
#include <stdexcept>
#include <mutex>
#include <condition_variable>
#include <iostream>

class JobQueueFullException : public std::runtime_error
{
public:
    // Constructor accepting const char*
    JobQueueFullException(const char *msg) : std::runtime_error(msg) {}
};

class JobQueueEmptyException : public std::runtime_error
{
public:
    JobQueueEmptyException(const char *msg) : std::runtime_error(msg) {}
};

template <typename T>
struct Node
{
    T value;
    Node *next;

    Node(const T &val) : value(val), next(nullptr) {};
};

// Queue implemented with linked list
template <typename T>
class JobQueue
{
public:
    JobQueue(int capacity) : m_capacity{capacity}, m_size{0}, m_front{nullptr}, m_back{nullptr} {};

    // non-blocking - once a push fails, the caller is responsible for calling and trying again
    void push(T item)
    {
        // throw if full
        // m_capacity doesn't change, just m_size
        // so need to lock m_size
        std::lock_guard<std::mutex> lock(m_mut);
        if (m_size == m_capacity)
        {
            throw JobQueueFullException("Job queue full");
        }

        Node<T> *newNode = new Node<T>(item);

        if (this->empty())
        {
            m_front = newNode;
            m_back = newNode; // need to lcok front and back
        }
        else
        {
            m_back->next = newNode;
            m_back = newNode;
        }
        m_size++;
    };

    // waits until we are not full before pushing.
    void wait_and_push(const T &item)
    {
        // std::cout << "wait and push" << "\n";
        // lock the mutex, and pass to cv
        std::unique_lock<std::mutex> lock(m_mut);
        m_not_full.wait(lock, [this]
                        { return !this->full(); }); // should only run when predicate resolves to true

        Node<T> *newNode = new Node<T>(item);

        if (this->empty())
        {
            m_front = newNode;
            m_back = newNode; // need to lcok front and back
        }
        else
        {
            m_back->next = newNode;
            m_back = newNode;
        }
        m_size++;

        // unlock before notifying consumers
        // this is an optional optimization - ensures that
        // once waiting threads are awoken, they will immediately have access to the mutex
        // withiout having to wait for this function to return and for the unique_lock to go out of scope.
        lock.unlock();
        m_not_empty.notify_one();
    }

    T pop()
    {
        std::lock_guard<std::mutex> lock(m_mut);
        if (this->empty())
        {
            throw JobQueueEmptyException("Job queue empty");
        }

        Node<T> *oldHead = m_front; // this needds to be locked, but back also needsto be lockedcause both could be the same
        T val = oldHead->value;
        m_size--;

        if (m_size == 0)
        {
            m_front = nullptr;
            m_back = nullptr;
        }
        else
        {
            m_front = oldHead->next;
        }

        delete oldHead;

        return val;
    };

    // blocks if queue is empty, wakes it up once something has been added to queue
    T wait_and_pop()
    {
        // lock the mutex, and pass lock to cv
        std::unique_lock<std::mutex> lock(m_mut);
        m_not_empty.wait(lock, [this]
                         { return !this->empty(); }); // define predicate for which we can exit the while loop.

        Node<T> *oldHead = m_front; // this needds to be locked, but back also needsto be lockedcause both could be the same
        T val = oldHead->value;
        m_size--;

        if (m_size == 0)
        {
            m_front = nullptr;
            m_back = nullptr;
        }
        else
        {
            m_front = oldHead->next;
        }

        delete oldHead;

        // unlock mutex, and notify producer threads that we are now not full
        lock.unlock();
        m_not_full.notify_one();

        return val;
    }

    // don't have to lock because this is not a CS
    int size()
    {
        return m_size;
    }

    // don't have to lock because this is not a CS
    bool empty()
    {
        return m_size == 0;
    }

    bool full()
    {
        return m_size == m_capacity;
    }

    // push and pop
    ~JobQueue()
    {
        Node<T> *cur = m_front;
        while (cur)
        {
            Node<T> *nxt = cur->next;
            delete cur;
            cur = nxt;
        }
    };

private:
    // INVARIANTS:
    // if m_front == m_back, the queue is either full or empty.
    // check m_size adn m_capacity.
    std::mutex m_mut;                    // shared mutex for a Queue instance
    std::condition_variable m_not_full;  // only responsible for monitoring threads that are waiting on this cv, and waking them if notified
    std::condition_variable m_not_empty; // for threads that require the queue to be NOT EMPTY (ie. consumers)

    // the below are all shared data so we use a single mutex to lock all of them at ocne for a critical section.
    int m_capacity; // max cap
    int m_size;
    Node<T> *m_front;
    Node<T> *m_back;
};