/*
 * Copyright 2020 ObjectBox Ltd. All rights reserved.
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
#include <cmath>
#include <cstdlib>
#include <iostream>

#include "StopWatch.h"
#include "objectbox-cpp.h"
#include "objectbox-model.h"
#include "ts-data-model-cpp.obx.h"

using namespace objectbox;          // util
using namespace objectbox::tsdemo;  // our generated code

std::vector<SensorValues> createSensorValueData(int64_t now, int dataCount);

void putAndPrintNamedTimeRanges(int64_t now, obx::Box<NamedTimeRange>& boxNTR);

int64_t millisSinceEpoch() {
    auto time = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(time).count();
}

int main(int argc, char* args[]) {
    std::cout << "ObjectBox TS demo using ObjectBox library " << obx_version_string()
              << " (core: " << obx_version_core_string() << ")" << std::endl;

    if (!obx_supports_time_series()) {
        std::cout << "This time series demo requires ObjectBox TS, a special time series edition." << std::endl;
        std::cout << "To get ObjectBox TS, please visit https://objectbox.io/time-series-database/." << std::endl;
        exit(1);
    }

    // Init ObjectBox store and boxes
    obx::Store::Options options(create_obx_model());
    obx::Store store(options);
    obx::Box<NamedTimeRange> boxNTR(store);
    obx::Box<SensorValues> boxSV(store);
    std::cout << "ObjectBox store opened" << std::endl;

    // Create some SensorValues dummy data and put it in the database (while measuring the time for put)
    int64_t start = millisSinceEpoch();
    int dataCount = 1000000;
    std::vector<SensorValues> values = createSensorValueData(start, dataCount);
    StopWatch stopWatch;
    boxSV.put(values);
    std::cout << "Put " << dataCount << " objects in " << stopWatch.durationForLog() << std::endl;
    values.clear();  // Free memory for putted objects (optional)

    putAndPrintNamedTimeRanges(start, boxNTR);

    // Query objects in a certain one second time range
    obx::QueryBuilder<SensorValues> queryBuilder = boxSV.query();
    obx_qb_int_between(queryBuilder.cPtr(), SensorValues_::time, start + 1000, start + 1999);
    obx::Query<SensorValues> query = queryBuilder.build();
    stopWatch.reset();
    std::vector<std::unique_ptr<SensorValues>> result = query.find();
    std::cout << "Time range query completed in " << stopWatch.durationForLog() << " (" << result.size()
              << " resulting objects)" << std::endl;

    return 0;
}

std::vector<SensorValues> createSensorValueData(int64_t now, int dataCount) {
    std::vector<SensorValues> values;
    values.reserve(dataCount);
    double deltaDelta = 0.000001;
    double delta = 0.0;
    double value = 0.0;
    for (int i = 1; i <= dataCount; ++i) {
        delta += deltaDelta;
        value += delta;
        if ((delta > 0.01 && deltaDelta > 0) || (delta < -0.01 && deltaDelta < 0)) {
            deltaDelta = -deltaDelta;
        }
        if (i % 10000 == 0) std::cout << "index " << i << ": v=" << value << ", delta=" << delta << std::endl;

        SensorValues sensorValues{OBX_ID_NEW,
                                  now - 1000 + i * 20,
                                  19.5 + value,
                                  23.3 + value * cos(value),
                                  42.75 + value * sin(value),
                                  0.80 - value,
                                  0.7 - value,
                                  0.6 - value,
                                  0.5 - value};
        values.emplace_back(sensorValues);
    }
    return values;
}

void putAndPrintNamedTimeRanges(int64_t now, obx::Box<NamedTimeRange>& boxNTR) {
    NamedTimeRange timeRangeGreen{OBX_ID_NEW, now - 1000, now + 1000, "green"};
    obx_id id = boxNTR.put(timeRangeGreen);
    std::cout << "New ID for time range 'green': " << id << std::endl;
    std::cout << "Object ID set to: " << timeRangeGreen.id << std::endl;

    NamedTimeRange timeRangeRed{OBX_ID_NEW, now + 1000, now + 2000, "red"};
    id = boxNTR.put(timeRangeRed);
    std::cout << "New ID for time range 'red': " << id << std::endl;

    std::cout << "Total time range count: " << boxNTR.count() << std::endl;

    std::unique_ptr<NamedTimeRange> object = boxNTR.get(id);
    if (object) {
        std::cout << "Read object: " << object->id << ", name: " << object->name << std::endl;
    }
}
