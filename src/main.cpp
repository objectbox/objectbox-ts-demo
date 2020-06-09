#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iostream>

#include "objectbox-ext.h"
#include "objectbox-model.h"
#include "objectbox.h"
#include "ts-data-model.obx.h"

using namespace objectbox::tsdemo;

int64_t millisSinceEpoch() {
    auto time = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(time).count();
}

int main(int argc, char* args[]) {
    std::cout << "ObjectBox TS demo using version " << obx_version_string() << "(core: " << obx_version_core_string()
              << ")" << std::endl;

    objectbox::Store store(create_obx_model());
    objectbox::Box<NamedTimeRange_> boxNTR(store);
    objectbox::Box<SensorValues_> boxSV(store);

    // TODO obx_opt_max_db_size_in_kb(opt, 10 * 1024 * 1024);

    std::cout << "ObjectBox store opened" << std::endl;

    int64_t now = millisSinceEpoch();

    int dataCount = 1000000;
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
    int64_t putStart = millisSinceEpoch();
    boxSV.put(values);
    std::cout << "Put " << dataCount << " objects in " << millisSinceEpoch() - putStart << " ms" << std::endl;

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
    return 0;
}
