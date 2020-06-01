#include <chrono>
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
    std::cout << "ObjectBox TS demo using version " << obx_version_string()
              << "(core: " << obx_version_core_string() << ")" << std::endl;

    objectbox::Store store(create_obx_model());
    objectbox::Box<NamedTimeRange_> boxNTR(store);
    objectbox::Box<SensorValues_> boxSV(store);

    // TODO obx_opt_max_db_size_in_kb(opt, 10 * 1024 * 1024);

    std::cout << "ObjectBox store opened" << std::endl;

    int64_t now = millisSinceEpoch();

    int dataCount = 100000;
    std::vector<SensorValues> values;
    values.reserve(dataCount);
    for (int i = 0; i < dataCount; ++i) {
        double d = (i % 2000) - 1000 * 0.005;
        SensorValues sensorValues{OBX_ID_NEW, now - 1000 + i, 19.5 + d, 23.3 + d, 42.75 + d,
                                  0.80 - d,   0.7 - d,        0.6 - d,  0.5 - d};
        values.emplace_back(sensorValues);
    }
    boxSV.put(values);
    std::cout << "Time taken: " << millisSinceEpoch() - now << " ms" << std::endl;

    NamedTimeRange namedTimeRange{OBX_ID_NEW, now - 1000, now + 1000, "green"};
    obx_id id = boxNTR.put(namedTimeRange);
    std::cout << "New ID: " << id << std::endl;
    std::cout << "Object ID set to: " << namedTimeRange.id << std::endl;
    std::cout << "Total count: " << boxNTR.count() << std::endl;

    std::unique_ptr<NamedTimeRange> object = boxNTR.get(id);
    if (object) {
        std::cout << "Read object: " << object->id << ", name: " << object->name << std::endl;
    }
    return 0;
}
