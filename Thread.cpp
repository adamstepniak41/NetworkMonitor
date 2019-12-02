//
// Created by adam on 01/12/2019.
//

#include <functional>
#include "Thread.h"

void Thread::Start() {
    OnThreadStarting();
    m_runThread = true;
    m_mainThread.reset(new std::thread(std::bind(&Thread::MainLoop, this)));
}

void Thread::Stop() {
    m_runThread = false;
    m_mainThread->join();
}
