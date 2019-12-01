#pragma once

#include <atomic>
#include <memory>
#include <thread>

class Thread {
public:
    void Start();
    void Stop();
protected:
    virtual void MainLoop() = 0;

    std::atomic<bool> m_runThread;
    std::unique_ptr<std::thread> m_mainThread;
};
