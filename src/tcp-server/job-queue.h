#pragma once
#include "../types/task.h"
#include <stdexcept>
#include <mutex>

// Define a new exception class that
// inherits from std::exception
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

// 1) NAIVE IMPLEMENTATION - NOT THREAD SAFE
// using vector
// push -> vector.push_back(); if queue is full, throw error
// pop -> vector.erase(vector.begin()); if queue is empty, throw error

// implement a very naive queue with locking.
// try using our own queue
template <typename T>
struct Node
{
    T value;
    Node *next;

    Node(const T &val) : value(val), next(nullptr) {};
};

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
    std::mutex m_mut; // shared mutex for a Queue instance

    // the below are all shared data so we use a single mutex to lock all of them at ocne for a critical section.
    int m_capacity; // max cap
    int m_size;
    Node<T> *m_front;
    Node<T> *m_back;
};