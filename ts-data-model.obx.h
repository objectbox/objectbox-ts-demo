// Code generated by ObjectBox; DO NOT EDIT.

#ifndef TS_DATA_MODEL_OBX_H
#define TS_DATA_MODEL_OBX_H

#include <cstdint>
#include <string>
#include <vector>

#include "flatbuffers/flatbuffers.h"
#include "objectbox.h"

namespace objectbox {
namespace tsdemo {
struct NamedTimeRange {
    uint64_t id;
    int64_t begin;
    int64_t end;
    std::string name;
};

struct NamedTimeRange_ {
    static const obx_schema_id ENTITY_ID = 1;
    static const obx_schema_id id = 1;
    static const obx_schema_id begin = 2;
    static const obx_schema_id end = 3;
    static const obx_schema_id name = 4;
};

class NamedTimeRangeSerializer {
public:
    /// Write given object to the FlatBufferBuilder
    static void toFlatBuffer(flatbuffers::FlatBufferBuilder& fbb, const NamedTimeRange& object) {
        fbb.Clear();
        auto offsetname = fbb.CreateString(object.name);
        flatbuffers::uoffset_t fbStart = fbb.StartTable();
        fbb.TrackField(4, fbb.PushElement<uint64_t>(object.id));
        fbb.TrackField(6, fbb.PushElement<int64_t>(object.begin));
        fbb.TrackField(8, fbb.PushElement<int64_t>(object.end));
        fbb.AddOffset(10, offsetname);
        flatbuffers::Offset<flatbuffers::Table> offset;
        offset.o = fbb.EndTable(fbStart);
        fbb.Finish(offset);
    }

    /// Read an object from a valid FlatBuffer
    static NamedTimeRange fromFlatBuffer(const void* data, size_t size) {
        NamedTimeRange object;
        fromFlatBuffer(data, size, object);
        return object;
    }

    /// Read an object from a valid FlatBuffer
    static std::unique_ptr<NamedTimeRange> newFromFlatBuffer(const void* data, size_t size) {
        auto object = std::unique_ptr<NamedTimeRange>(new NamedTimeRange());
        fromFlatBuffer(data, size, *object);
        return object;
    }

    /// Read an object from a valid FlatBuffer
    static void fromFlatBuffer(const void* data, size_t size, NamedTimeRange& outObject) {
        const auto* table = flatbuffers::GetRoot<flatbuffers::Table>(data);
        assert(table);
        outObject.id = table->GetField<uint64_t>(4, 0);
        outObject.begin = table->GetField<int64_t>(6, 0);
        outObject.end = table->GetField<int64_t>(8, 0);
        {
            auto* ptr = table->GetPointer<const flatbuffers::Vector<char>*>(10);
            if (ptr) outObject.name.assign(ptr->begin(), ptr->end());
        }
        
    }
};
}  // namespace tsdemo
}  // namespace objectbox

namespace objectbox {
namespace tsdemo {
struct SensorValues {
    uint64_t id;
    int64_t time;
    double temperatureOutside;
    double temperatureInside;
    double temperatureCpu;
    double loadCpu1;
    double loadCpu2;
    double loadCpu3;
    double loadCpu4;
};

struct SensorValues_ {
    static const obx_schema_id ENTITY_ID = 2;
    static const obx_schema_id id = 1;
    static const obx_schema_id time = 2;
    static const obx_schema_id temperatureOutside = 3;
    static const obx_schema_id temperatureInside = 4;
    static const obx_schema_id temperatureCpu = 5;
    static const obx_schema_id loadCpu1 = 6;
    static const obx_schema_id loadCpu2 = 7;
    static const obx_schema_id loadCpu3 = 8;
    static const obx_schema_id loadCpu4 = 9;
};

class SensorValuesSerializer {
public:
    /// Write given object to the FlatBufferBuilder
    static void toFlatBuffer(flatbuffers::FlatBufferBuilder& fbb, const SensorValues& object) {
        fbb.Clear();
        flatbuffers::uoffset_t fbStart = fbb.StartTable();
        fbb.TrackField(4, fbb.PushElement<uint64_t>(object.id));
        fbb.TrackField(6, fbb.PushElement<int64_t>(object.time));
        fbb.TrackField(8, fbb.PushElement<double>(object.temperatureOutside));
        fbb.TrackField(10, fbb.PushElement<double>(object.temperatureInside));
        fbb.TrackField(12, fbb.PushElement<double>(object.temperatureCpu));
        fbb.TrackField(14, fbb.PushElement<double>(object.loadCpu1));
        fbb.TrackField(16, fbb.PushElement<double>(object.loadCpu2));
        fbb.TrackField(18, fbb.PushElement<double>(object.loadCpu3));
        fbb.TrackField(20, fbb.PushElement<double>(object.loadCpu4));
        flatbuffers::Offset<flatbuffers::Table> offset;
        offset.o = fbb.EndTable(fbStart);
        fbb.Finish(offset);
    }

    /// Read an object from a valid FlatBuffer
    static SensorValues fromFlatBuffer(const void* data, size_t size) {
        SensorValues object;
        fromFlatBuffer(data, size, object);
        return object;
    }

    /// Read an object from a valid FlatBuffer
    static std::unique_ptr<SensorValues> newFromFlatBuffer(const void* data, size_t size) {
        auto object = std::unique_ptr<SensorValues>(new SensorValues());
        fromFlatBuffer(data, size, *object);
        return object;
    }

    /// Read an object from a valid FlatBuffer
    static void fromFlatBuffer(const void* data, size_t size, SensorValues& outObject) {
        const auto* table = flatbuffers::GetRoot<flatbuffers::Table>(data);
        assert(table);
        outObject.id = table->GetField<uint64_t>(4, 0);
        outObject.time = table->GetField<int64_t>(6, 0);
        outObject.temperatureOutside = table->GetField<double>(8, 0.0);
        outObject.temperatureInside = table->GetField<double>(10, 0.0);
        outObject.temperatureCpu = table->GetField<double>(12, 0.0);
        outObject.loadCpu1 = table->GetField<double>(14, 0.0);
        outObject.loadCpu2 = table->GetField<double>(16, 0.0);
        outObject.loadCpu3 = table->GetField<double>(18, 0.0);
        outObject.loadCpu4 = table->GetField<double>(20, 0.0);
        
    }
};
}  // namespace tsdemo
}  // namespace objectbox

#endif  // TS_DATA_MODEL_OBX_H
