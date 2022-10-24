#include <MVP/MVPTimer.hpp>
#include <Utility/MVPUtility.hpp>

using namespace std;

MVPTimer::MVPTimer()
    : numTimer(0)
{}

void MVPTimer::run() {
    unsigned long currentMillis = MVPMillis();
    for (int i = 0; i < MAX_TIMERS; ++i) {
        if (!this->isValidTimer(i)) {
            continue;
        }
        if (currentMillis - this->timer[i].prevMillis < this->timer[i].delay) {
            continue;
        }
        unsigned long skipTimes = (currentMillis - this->timer[i].prevMillis) / this->timer[i].delay;
        // update time
        this->timer[i].prevMillis += this->timer[i].delay * skipTimes;
        // call callback
        if (!this->timer[i].enable) {
            continue;
        }
        this->timer[i].called = true;
    }
    
    for (int i = 0; i < MAX_TIMERS; ++i) {
        if (!this->timer[i].called) {
            continue;
        }
        if (this->timer[i].callback_p == nullptr) {
            this->timer[i].callback();
        }
        else {
            this->timer[i].callback_p(this->timer[i].param);
        }
        this->timer[i].called = false;
        if (this->timer[i].limit) {
            if (++this->timer[i].count >= this->timer[i].limit) {
                this->deleteTimer(i);
            }
        }
    }
}

int MVPTimer::setupTimer(unsigned long interval, TimerCallback_t cb, unsigned int limit) {
    int id = this->findTimerFree();
    if (id < 0) {
        return -1;
    }
    if (!interval) {
        return -1;
    }

    this->timer[id].delay = interval;
    this->timer[id].callback = cb;
    this->timer[id].callback_p = nullptr;
    this->timer[id].param = nullptr;
    this->timer[id].limit = limit;
    this->timer[id].count = 0;
    this->timer[id].enable = true;
    this->timer[id].called = false;
    this->timer[id].prevMillis = MVPMillis();
    this->numTimer++;
    return id;
}

int MVPTimer::setupTimer(unsigned long interval, TimerCallback_p_t cb, void* arg, unsigned int limit) {
    int id = this->findTimerFree();
    if (id < 0) {
        return -1;
    }
    if (!interval) {
        return -1;
    }

    this->timer[id].delay = interval;
    this->timer[id].callback = nullptr;
    this->timer[id].callback_p = cb;
    this->timer[id].param = arg;
    this->timer[id].limit = limit;
    this->timer[id].count = 0;
    this->timer[id].enable = true;
    this->timer[id].called = false;
    this->timer[id].prevMillis = MVPMillis();
    this->numTimer++;
    return id;
}

bool MVPTimer::changeInterval(unsigned int id, unsigned long interval) {
    if (id >= MAX_TIMERS) {
        return false;
    }
    if (!interval) {
        return false;
    }

    if (!this->isValidTimer(id)) {
        return false;
    }

    this->timer[id].delay = interval;
    this->timer[id].prevMillis = MVPMillis();
    return true;
}

void MVPTimer::restartTimer(unsigned int id) {
    if (id >= MAX_TIMERS) {
        return;
    }

    this->timer[id].prevMillis = MVPMillis();
}

void MVPTimer::executeNow(unsigned int id) {
    if (id >= MAX_TIMERS) {
        return;
    }

    this->timer[id].prevMillis = MVPMillis() - this->timer[id].delay;
}

void MVPTimer::deleteTimer(unsigned int id) {
    if (id >= MAX_TIMERS) {
        return;
    }
    
    if (!this->numTimer) {
        return;
    }

    if (this->isValidTimer(id)) {
        this->timer[id] = Timer_t();
        this->timer[id].prevMillis = MVPMillis();
        this->numTimer--;
    }
}

bool MVPTimer::isEnable(unsigned int id) {
    if (id >= MAX_TIMERS) {
        return false;
    }

    return this->timer[id].enable;
}

void MVPTimer::enable(unsigned int id) {
    if (id >= MAX_TIMERS) {
        return;
    }

    this->timer[id].enable = true;
}

void MVPTimer::disable(unsigned int id) {
    if (id >= MAX_TIMERS) {
        return;
    }

    this->timer[id].enable = false;
}

void MVPTimer::enableAll()
{
    for (int i = 0; i < MAX_TIMERS; ++i) {
        if (this->isValidTimer(i)) {
            this->timer[i].enable = true;
        }
    }
}

void MVPTimer::disableAll()
{
    for (int i = 0; i < MAX_TIMERS; ++i) {
        if (this->isValidTimer(i)) {
            this->timer[i].enable = false;
        }
    }
}

int MVPTimer::findTimerFree()
{
    if (this->numTimer >= MAX_TIMERS) {
        return -1;
    }

    for (int i = 0; i < MAX_TIMERS; ++i) {
        if (!this->isValidTimer(i)) {
            return i;
        }
    }

    return -1;
}