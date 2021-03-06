// **********************************************************************
//
// Copyright (c) 2003-2008 ZeroC, Inc. All rights reserved.
//
// This copy of Ice-E is licensed to you under the terms described in the
// ICEE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <IceE/RecMutex.h>
#include <IceE/Exception.h>
#include <IceE/MutexProtocol.h>

using namespace std;

IceUtil::RecMutex::RecMutex() :
    _count(0)
{
#ifdef _WIN32
    init(PrioNone);
#else
    init(getDefaultMutexProtocol());
#endif
}

IceUtil::RecMutex::RecMutex(MutexProtocol protocol) :
    _count(0)
{
    init(protocol);
}


#ifdef _WIN32

void
IceUtil::RecMutex::init(MutexProtocol)
{
    InitializeCriticalSection(&_mutex);
}

IceUtil::RecMutex::~RecMutex()
{
    assert(_count == 0);
    DeleteCriticalSection(&_mutex);
}

void
IceUtil::RecMutex::lock() const
{
    EnterCriticalSection(&_mutex);
    if(++_count > 1)
    {
        LeaveCriticalSection(&_mutex);
    }
}

bool
IceUtil::RecMutex::tryLock() const
{
    if(!TryEnterCriticalSection(&_mutex))
    {
        return false;
    }
    if(++_count > 1)
    {
        LeaveCriticalSection(&_mutex);
    }
    return true;
}

void
IceUtil::RecMutex::unlock() const
{
    if(--_count == 0)
    {
        LeaveCriticalSection(&_mutex);
    }
}

void
IceUtil::RecMutex::unlock(LockState& state) const
{
    state.count = _count;
    _count = 0;
    LeaveCriticalSection(&_mutex);
}

void
IceUtil::RecMutex::lock(LockState& state) const
{
    EnterCriticalSection(&_mutex);
    _count = state.count;
}

#else

void
IceUtil::RecMutex::init(MutexProtocol protocol)
{
#if defined(__linux) && !defined(__USE_UNIX98)
    const pthread_mutexattr_t attr = { PTHREAD_MUTEX_RECURSIVE_NP };
    int rc = pthread_mutex_init(&_mutex, &attr);
    if(rc != 0)
    {
        throw ThreadSyscallException(__FILE__, __LINE__, rc);
    }
#elif defined(VXWORKS)

    //memset(&_mutex, '\0',sizeof(_mutex));

    int rc = pthread_mutex_init(&_mutex,0);

    assert(rc == 0);

    if (rc != 0)

    {

        throw ThreadSyscallException(__FILE__,__LINE__, rc);

    }
#else // !defined(__linux) || defined(__USE_UNIX98)
    int rc;
    pthread_mutexattr_t attr;
    rc = pthread_mutexattr_init(&attr);
    if(rc != 0)
    {
        throw ThreadSyscallException(__FILE__, __LINE__, rc);
    }
#ifndef NDEBUG

   rc = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);

   assert(rc == 0);
    if(rc != 0)
    {
        throw ThreadSyscallException(__FILE__, __LINE__, rc);
    }
#endif
    
#if defined(_POSIX_THREAD_PRIO_INHERIT) && _POSIX_THREAD_PRIO_INHERIT > 0
    if(PrioInherit == protocol)
    {
        rc = pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT);
        if(rc != 0)
        {
            throw ThreadSyscallException(__FILE__, __LINE__, rc);
        }
    }
#endif

    rc = pthread_mutex_init(&_mutex, &attr);
    if(rc != 0)
    {
        throw ThreadSyscallException(__FILE__, __LINE__, rc);
    }

    rc = pthread_mutexattr_destroy(&attr);
    if(rc != 0)
    {
        throw ThreadSyscallException(__FILE__, __LINE__, rc);
    }
#endif
}

IceUtil::RecMutex::~RecMutex()
{
    assert(_count == 0);
    int rc = 0;
    rc = pthread_mutex_destroy(&_mutex);
    assert(rc == 0);
}

void
IceUtil::RecMutex::lock() const
{
    int rc = pthread_mutex_lock(&_mutex);
    if(rc != 0)
    {
        throw ThreadSyscallException(__FILE__, __LINE__, rc);
    }
    if(++_count > 1)
    {
        rc = pthread_mutex_unlock(&_mutex);
        assert(rc == 0);
    }
}

bool
IceUtil::RecMutex::tryLock() const
{
    int rc = pthread_mutex_trylock(&_mutex);
    bool result = (rc == 0);
    if(!result)
    {
        if(rc != EBUSY)
        {
            throw ThreadSyscallException(__FILE__, __LINE__, rc);
        }
    } 
    else if(++_count > 1)
    {
        rc = pthread_mutex_unlock(&_mutex);
        if(rc != 0)
        {
            throw ThreadSyscallException(__FILE__, __LINE__, rc);
        }
    }
    return result;
}

void
IceUtil::RecMutex::unlock() const
{
    if(--_count == 0)
    {
        int rc = 0; // Prevent warnings when NDEBUG is defined.
        rc = pthread_mutex_unlock(&_mutex);
        assert(rc == 0);
    }
}

void
IceUtil::RecMutex::unlock(LockState& state) const
{
    state.mutex = &_mutex;
    state.count = _count;
    _count = 0;
}

void
IceUtil::RecMutex::lock(LockState& state) const
{
    _count = state.count;
}

#endif

bool
IceUtil::RecMutex::willUnlock() const
{
    return _count == 1;
}
