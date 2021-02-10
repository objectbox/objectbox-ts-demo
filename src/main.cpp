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
#include <iomanip>
#include <iostream>

#include "StopWatch.h"
#include "objectbox-model.h"
#include "objectbox.hpp"
#include "ts-data-model.obx.hpp"

using namespace objectbox;          // util
using namespace objectbox::tsdemo;  // our generated code

std::vector<SensorValues> createSensorValueData(int64_t now, int dataCount);
void putAndPrintNamedTimeRanges(obx::Box<NamedTimeRange>& boxNTR, int64_t start);
void removeDataBefore(obx::Box<SensorValues> box, int64_t time);
void buildAndRunQueries(obx::Box<SensorValues>& box, int64_t start);
void printMinMaxTime(obx::Box<SensorValues>& box, int64_t start);

int64_t millisSinceEpoch() {
    auto time = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(time).count();
}

void putTimestamp(int64_t millis) {
    std::time_t timeSeconds = millis / 1000;
    std::tm* t = std::gmtime(&timeSeconds);
    std::cout << std::put_time(t, "%F %T");
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
    options.maxDbSizeInKb(10 * 1024 * 1024);  // 10 GB to allow data from several runs
    obx::Store store(options);
    obx::Box<NamedTimeRange> boxNTR(store);
    obx::Box<SensorValues> boxSV(store);
    std::cout << "ObjectBox store opened" << std::endl;

    int64_t start = millisSinceEpoch();
    boxNTR.removeAll();
    removeDataBefore(boxSV, start - 5000);  // Older than 5s
    // boxSV.removeAll();  // Or remove all if you prefer consistent data each run

    // Create some SensorValues dummy data and put it in the database (while measuring the time for put)
    int dataCount = 1000000;
    std::vector<SensorValues> values = createSensorValueData(start, dataCount);
    StopWatch stopWatch;
    boxSV.put(values);
    std::cout << "Put " << dataCount << " objects in " << stopWatch.durationForLog() << std::endl;
    values.clear();  // Free memory for putted objects (optional)

    putAndPrintNamedTimeRanges(boxNTR, start);

    printMinMaxTime(boxSV, start);

    buildAndRunQueries(boxSV, start);

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
        if (i % 100000 == 0) std::cout << "index " << i << ": v=" << value << ", delta=" << delta << std::endl;

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

void putAndPrintNamedTimeRanges(obx::Box<NamedTimeRange>& boxNTR, int64_t start) {
    NamedTimeRange timeRangeGreen{OBX_ID_NEW, start - 1000, start + 1000, "green"};
    obx_id id = boxNTR.put(timeRangeGreen);
    std::cout << "New ID for time range 'green': " << id << std::endl;
    std::cout << "Object ID set to: " << timeRangeGreen.id << std::endl;

    NamedTimeRange timeRangeRed{OBX_ID_NEW, start + 1000, start + 2000, "red"};
    id = boxNTR.put(timeRangeRed);
    std::cout << "New ID for time range 'red': " << id << std::endl;

    std::cout << "Total time range count: " << boxNTR.count() << std::endl;

    std::unique_ptr<NamedTimeRange> object = boxNTR.get(id);
    if (object) {
        std::cout << "Read object: " << object->id << ", name: " << object->name << std::endl;
    }
}

void printMinMaxTime(obx::Box<SensorValues>& box, int64_t start) {
    std::cout << "Start time: ";
    putTimestamp(start);

    StopWatch stopWatch;
    int64_t timeMin{0}, timeMax{0};
    // We could also get the IDs, but we pass nullptr instead as we only print the values
    box.timeSeriesMinMax(nullptr, &timeMin, nullptr, &timeMax);
    std::cout << std::endl << "Min time value in database: ";
    putTimestamp(timeMin);
    std::cout << std::endl << "Max time value in database: ";
    putTimestamp(timeMax);
    std::cout << std::endl << "Got min/max in " << stopWatch.durationForLog() << std::endl;
}

void buildAndRunQueries(obx::Box<SensorValues>& box, int64_t start) {
    // Query objects in a certain one second time range
    {
        obx::QueryBuilder<SensorValues> qbRange = box.query();
        qbRange.with(SensorValues_::time.between(start + 1000, start + 1999));
        obx::Query<SensorValues> query = qbRange.build();  // Note: query object can be re-used (without its builder)
        StopWatch stopWatch;
        std::vector<std::unique_ptr<SensorValues>> result = query.find();
        std::cout << "Time range query completed in " << stopWatch.durationForLog() << " (" << result.size()
                  << " resulting objects)" << std::endl;
    }

    // Query objects using a link to a entity type that defines a time range (NamedTimeRange)
    {
        obx::QueryBuilder<SensorValues> qbLink = box.query();
        obx::QueryBuilder<NamedTimeRange> qbNamedTimeRange =
            qbLink.linkTime<NamedTimeRange>(NamedTimeRange_::begin, NamedTimeRange_::end);
        qbNamedTimeRange.with(NamedTimeRange_::name.equals("green"));
        obx::Query<SensorValues> query = qbLink.build();  // Note: query object can be re-used (without its builder)

        StopWatch stopWatch;
        std::vector<std::unique_ptr<SensorValues>> result = query.find();
        std::cout << "Named time range query (linked) completed in " << stopWatch.durationForLog() << " ("
                  << result.size() << " resulting objects)" << std::endl;
    }
}

void removeDataBefore(obx::Box<SensorValues> box, int64_t time) {
    obx::QueryBuilder<SensorValues> queryBuilder = box.query();
    queryBuilder.with(SensorValues_::time.lessThan(time));
    StopWatch stopWatch;
    size_t removeCount = queryBuilder.build().remove();
    std::cout << "Removed old objects in " << stopWatch.durationForLog() << " (" << removeCount << " objects)"
              << std::endl;

    std::cout << "Total objects in DB: " << box.count() << std::endl;
}
