// ============================================================================
// Copyright (c) 2009-2010 Faustino Frechilla
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
/// @file safe_queue.h
/// @brief Definition of a thread-safe queue based on glib system calls
/// It internally contains a std::queue which is protected from concurrent
/// access by glib mutexes and conditional variables
///
/// @author Faustino Frechilla
/// @history
/// Ref  Who                 When         What
///      Faustino Frechilla 04-May-2009 Original development (based on pthreads)
///      Faustino Frechilla 19-May-2010 Ported to glib. Removed pthread dependency
/// @endhistory
///
// ============================================================================

#ifndef __SAFEQUEUE_H__
#define __SAFEQUEUE_H__

#include <glib.h>
#include <queue>
#include <limits> // std::numeric_limits<>::max

#define SAFE_QUEUE_DEFAULT_MAX_SIZE std::numeric_limits<std::size_t >::max()

/// @brief thread-safe queue
/// It uses a mutex+condition variables to protect the internal queue
/// implementation. Inserting or reading elements use the same mutex
template <typename T>
class SafeQueue
{
public:
    SafeQueue(std::size_t a_maxSize = SAFE_QUEUE_DEFAULT_MAX_SIZE);
    ~SafeQueue();

    /// @brief Check if the queue is empty
    /// This call can block if another thread owns the lock that protects the
    /// queue
    /// @return true if the queue is empty. False otherwise
    bool IsEmpty();

    /// @brief inserts an element into queue queue
    /// This call can block if another thread owns the lock that protects the
    /// queue. If the queue is full The thread will be blocked in this queue
    /// until someone else gets an element from the queue
    /// @param element to insert into the queue
    /// @return True if the elem was successfully inserted into the queue.
    ///         False otherwise
    bool Push(const T &a_elem);

    /// @brief inserts an element into queue queue
    /// This call can block if another thread owns the lock that protects the
    /// queue. If the queue is full The call will return false and the element
    /// won't be inserted
    /// @param element to insert into the queue
    /// @return True if the elem was successfully inserted into the queue.
    ///         False otherwise
    bool TryPush(const T &a_elem);

    /// @brief extracts an element from the queue (and deletes it from the q)
    /// If the queue is empty this call will block the thread until there is
    /// something in the queue to be extracted
    /// @param a reference where the element from the queue will be saved to
    void Pop(T &out_data);

    /// @brief extracts an element from the queue (and deletes it from the q)
    /// This call gets the block that protects the queue. It will extract the
    /// element from the queue only if there are elements in it
    /// @param reference to the variable where the result will be saved
    /// @return True if the element was retrieved from the queue.
    ///         False if the queue was empty
    bool TryPop(T &out_data);

    /// @brief extracts an element from the queue (and deletes it from the q)
    /// If the queue is empty this call will block the thread until there
    /// is something in the queue to be extracted or until the timer
    /// (2nd parameter) expires
    /// @param reference to the variable where the result will be saved
    /// @param microsecondsto wait before returning if the queue was empty
    /// @return True if the element was retrieved from the queue.
    ///         False if the timeout was reached
    bool TimedWaitPop(T &data, glong microsecs);

protected:
    std::queue<T> m_theQueue;
    /// maximum number of elements for the queue
    std::size_t m_maximumSize;
    /// Mutex to protect the queue
    GMutex* m_mutex;
    /// Conditional variable to wake up threads
    GCond*  m_cond;
};

// include the implementation file
#include "safe_queue_impl.h"

#endif // __SAFEQUEUE_H__

