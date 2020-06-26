/*
 * Copyright 2017-2020 ObjectBox Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <chrono>

#include "StopWatch.h"

using namespace objectbox;

std::chrono::steady_clock::duration StopWatch::duration() const { return std::chrono::steady_clock::now() - start_; }

uint64_t StopWatch::durationInNanos() const {
    return (uint64_t) std::chrono::duration_cast<std::chrono::nanoseconds>(duration()).count();
}

uint64_t StopWatch::durationInMicros() const {
    return (uint64_t) std::chrono::duration_cast<std::chrono::microseconds>(duration()).count();
}

uint64_t StopWatch::durationInMillis() const {
    return (uint64_t) std::chrono::duration_cast<std::chrono::milliseconds>(duration()).count();
}

std::string StopWatch::durationForLog() const { return durationForLog(durationInNanos()); }

std::string StopWatch::durationForLog(uint64_t nanos) {
    if (nanos >= 10000000000L) {  // >= 10 s
        std::string value = std::to_string((nanos + 500 * 1000000L) / (1000 * 1000000L));
        value += " s";
        return value;
    } else if (nanos >= 1000000 * 10) {  // >= 10 ms
        std::string value = std::to_string((nanos + 500000) / 1000000);
        value += " ms";
        return value;
    } else if (nanos >= 10000) {  // >= 0.01 ms
        uint64_t micros10 = (nanos + 5000) / 10000;
        std::string value = std::to_string(micros10 / 100);
        value += ".";
        micros10 %= 100;
        if (micros10 < 10) value += "0";
        value += std::to_string(micros10);
        value += " ms";
        return value;
    } else {
        std::string value = std::to_string(nanos);
        value += " ns";
        return value;
    }
}

void StopWatch::reset() { start_ = std::chrono::steady_clock::now(); }
