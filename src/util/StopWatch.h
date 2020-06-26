
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

#ifndef OBJECTBOX_STOPWATCH_H
#define OBJECTBOX_STOPWATCH_H

#include <chrono>
#include <cstdint>
#include <string>

namespace objectbox {

/// Measures durations using a steady clock (e.g. std::chrono::steady_clock, not wall clock that might change).
class StopWatch {
    std::chrono::steady_clock::time_point start_ = std::chrono::steady_clock::now();

    std::chrono::steady_clock::duration duration() const;

public:
    /// Sets the start time to now
    StopWatch() = default;

    /// Sets the start time to now
    void reset();

    /// Duration from start time until now
    uint64_t durationInNanos() const;

    /// Duration from start time until now
    uint64_t durationInMicros() const;

    /// Duration from start time until now
    uint64_t durationInMillis() const;

    /// Duration from start time until now in a format appropriate to log (e.g. 5 ms, 0.031 ms, 987 ns)
    std::string durationForLog() const;

    /// For testing
    static std::string durationForLog(uint64_t durationInNanos);
};

}  // namespace objectbox

#endif  // OBJECTBOX_STOPWATCH_H
