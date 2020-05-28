#include <chrono>
#include <cstdlib>
#include <iostream>

#include "objectbox-model.h"
#include "objectbox.h"
#include "ts-data-model.obx.h"

using namespace objectbox::tsdemo;

int64_t millisSinceEpoch() {
    auto time = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(time).count();
}

obx_err printError(const char* hint = nullptr) {
    if (hint) printf("Error: %s\n", hint);
    printf("Last error: %d, %d (%s)\n", obx_last_error_code(), obx_last_error_secondary(),
           obx_last_error_message());
    return obx_last_error_code();
}

[[noreturn]] void printErrorAndExit(const char* hint = nullptr) { exit(printError(hint)); }

obx_id put(OBX_store* store, flatbuffers::FlatBufferBuilder& fbb,
           const NamedTimeRange& namedTimeRange) {
    fbb.Clear();
    NamedTimeRangeSerializer::toFlatBuffer(fbb, namedTimeRange);
    OBX_box* box = obx_box(store, 1);
    if (box == nullptr) printErrorAndExit("box");
    return obx_box_put_object(box, fbb.GetBufferPointer(), fbb.GetSize(), OBXPutMode_PUT);
}

void put(OBX_store* store, flatbuffers::FlatBufferBuilder& fbb,
         const std::vector<SensorValues>& values) {
    OBX_box* box = obx_box(store, 2);
    if (box == nullptr) printErrorAndExit("box2");

    OBX_txn* txn = obx_txn_write(store);
    if (txn == nullptr) printErrorAndExit("write transaction");
    for (auto& value : values) {
        fbb.Clear();
        SensorValuesSerializer::toFlatBuffer(fbb, value);
        obx_id id = obx_box_put_object(box, fbb.GetBufferPointer(), fbb.GetSize(), OBXPutMode_PUT);
        if (id == 0) printErrorAndExit("put");
    }
    obx_txn_success(txn);
}

std::unique_ptr<NamedTimeRange> get(OBX_store* store, obx_id id) {
    OBX_box* box = obx_box(store, 1);
    if (box == nullptr) printErrorAndExit("box");

    OBX_txn* txn = obx_txn_read(store);
    if (txn == nullptr) printErrorAndExit("transaction");

    std::unique_ptr<NamedTimeRange> result;
    void* data = nullptr;
    size_t size = 0;
    obx_err err = obx_box_get(box, id, &data, &size);
    if (err == OBX_SUCCESS) {
        NamedTimeRange object = NamedTimeRangeSerializer::fromFlatBuffer(data, size);
        result.reset(new NamedTimeRange(std::move(object)));
    } else if (err != OBX_NOT_FOUND) {
        obx_txn_close(txn);
        throw std::runtime_error("Could not get");  // Use our own ex type
    }

    obx_txn_close(txn);

    return result;
}

int main(int argc, char* args[]) {
    std::cout << "ObjectBox TS demo using version " << obx_version_string()
              << "(core: " << obx_version_core_string() << ")" << std::endl;

    OBX_model* model = create_obx_model();
    if (!model) printErrorAndExit("model");

    OBX_store_options* opt = obx_opt();
    obx_opt_max_db_size_in_kb(opt, 10 * 1024 * 1024);
    obx_opt_model(opt, model);
    OBX_store* store = obx_store_open(opt);
    if (!store) printErrorAndExit("store");

    std::cout << "ObjectBox store opened" << std::endl;

    flatbuffers::FlatBufferBuilder fbb;

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
    put(store, fbb, values);

    NamedTimeRange namedTimeRange{OBX_ID_NEW, now - 1000, now + 1000, "green"};
    obx_id id = put(store, fbb, namedTimeRange);
    std::cout << "New ID: " << id << std::endl;

    OBX_box* box = obx_box(store, 1);
    if (box == nullptr) printErrorAndExit("box");

    uint64_t count = 0;
    obx_box_count(box, 0, &count);
    std::cout << "Total count: " << count << std::endl;

    std::unique_ptr<NamedTimeRange> object = get(store, id);
    if (object) {
        std::cout << "Read object: " << object->id << ", name: " << object->name << std::endl;
    }

    if (obx_store_close(store)) printErrorAndExit("store close");
    return 0;
}
