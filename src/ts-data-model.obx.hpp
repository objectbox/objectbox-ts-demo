// Code generated by ObjectBox; DO NOT EDIT.

#pragma once

#include <cstdbool>
#include <cstdint>

#include "flatbuffers/flatbuffers.h"
#include "objectbox.h"
#include "objectbox.hpp"


namespace objectbox {
namespace tsdemo {
struct NamedTimeRange_;

struct NamedTimeRange {
    using _OBX_MetaInfo = NamedTimeRange_;
    
    uint64_t id;
    int64_t begin;
    int64_t end;
    std::string name;
};

struct NamedTimeRange_ {
    static const obx::Property<NamedTimeRange, OBXPropertyType_Long> id;
    static const obx::Property<NamedTimeRange, OBXPropertyType_Date> begin;
    static const obx::Property<NamedTimeRange, OBXPropertyType_Date> end;
    static const obx::Property<NamedTimeRange, OBXPropertyType_String> name;

    static constexpr obx_schema_id entityId() { return 1; }

    static void setObjectId(NamedTimeRange& object, obx_id newId) { object.id = newId; }

    /// Write given object to the FlatBufferBuilder
    static void toFlatBuffer(flatbuffers::FlatBufferBuilder& fbb, const NamedTimeRange& object);

    /// Read an object from a valid FlatBuffer
    static NamedTimeRange fromFlatBuffer(const void* data, size_t size);

    /// Read an object from a valid FlatBuffer
    static std::unique_ptr<NamedTimeRange> newFromFlatBuffer(const void* data, size_t size);

    /// Read an object from a valid FlatBuffer
    static void fromFlatBuffer(const void* data, size_t size, NamedTimeRange& outObject);
};
}  // namespace tsdemo
}  // namespace objectbox


namespace objectbox {
namespace tsdemo {
struct SensorValues_;

/// Time series object
struct SensorValues {
    using _OBX_MetaInfo = SensorValues_;
    
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
    static const obx::Property<SensorValues, OBXPropertyType_Long> id;
    static const obx::Property<SensorValues, OBXPropertyType_Date> time;
    static const obx::Property<SensorValues, OBXPropertyType_Double> temperatureOutside;
    static const obx::Property<SensorValues, OBXPropertyType_Double> temperatureInside;
    static const obx::Property<SensorValues, OBXPropertyType_Double> temperatureCpu;
    static const obx::Property<SensorValues, OBXPropertyType_Double> loadCpu1;
    static const obx::Property<SensorValues, OBXPropertyType_Double> loadCpu2;
    static const obx::Property<SensorValues, OBXPropertyType_Double> loadCpu3;
    static const obx::Property<SensorValues, OBXPropertyType_Double> loadCpu4;

    static constexpr obx_schema_id entityId() { return 2; }

    static void setObjectId(SensorValues& object, obx_id newId) { object.id = newId; }

    /// Write given object to the FlatBufferBuilder
    static void toFlatBuffer(flatbuffers::FlatBufferBuilder& fbb, const SensorValues& object);

    /// Read an object from a valid FlatBuffer
    static SensorValues fromFlatBuffer(const void* data, size_t size);

    /// Read an object from a valid FlatBuffer
    static std::unique_ptr<SensorValues> newFromFlatBuffer(const void* data, size_t size);

    /// Read an object from a valid FlatBuffer
    static void fromFlatBuffer(const void* data, size_t size, SensorValues& outObject);
};
}  // namespace tsdemo
}  // namespace objectbox

