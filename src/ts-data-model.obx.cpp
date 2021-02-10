// Code generated by ObjectBox; DO NOT EDIT.

#include "ts-data-model.obx.hpp"

const obx::Property<objectbox::tsdemo::NamedTimeRange, OBXPropertyType_Long> objectbox::tsdemo::NamedTimeRange_::id(1);
const obx::Property<objectbox::tsdemo::NamedTimeRange, OBXPropertyType_Date> objectbox::tsdemo::NamedTimeRange_::begin(2);
const obx::Property<objectbox::tsdemo::NamedTimeRange, OBXPropertyType_Date> objectbox::tsdemo::NamedTimeRange_::end(3);
const obx::Property<objectbox::tsdemo::NamedTimeRange, OBXPropertyType_String> objectbox::tsdemo::NamedTimeRange_::name(4);

void objectbox::tsdemo::NamedTimeRange_::toFlatBuffer(flatbuffers::FlatBufferBuilder& fbb, const objectbox::tsdemo::NamedTimeRange& object) {
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

objectbox::tsdemo::NamedTimeRange objectbox::tsdemo::NamedTimeRange_::fromFlatBuffer(const void* data, size_t size) {
    objectbox::tsdemo::NamedTimeRange object;
    fromFlatBuffer(data, size, object);
    return object;
}

std::unique_ptr<objectbox::tsdemo::NamedTimeRange> objectbox::tsdemo::NamedTimeRange_::newFromFlatBuffer(const void* data, size_t size) {
    auto object = std::unique_ptr<objectbox::tsdemo::NamedTimeRange>(new objectbox::tsdemo::NamedTimeRange());
    fromFlatBuffer(data, size, *object);
    return object;
}

void objectbox::tsdemo::NamedTimeRange_::fromFlatBuffer(const void* data, size_t, objectbox::tsdemo::NamedTimeRange& outObject) {
    const auto* table = flatbuffers::GetRoot<flatbuffers::Table>(data);
    assert(table);
    outObject.id = table->GetField<uint64_t>(4, 0);
    outObject.begin = table->GetField<int64_t>(6, 0);
    outObject.end = table->GetField<int64_t>(8, 0);
    {
        auto* ptr = table->GetPointer<const flatbuffers::String*>(10);
        if (ptr) outObject.name.assign(ptr->c_str());
    }
    
}

const obx::Property<objectbox::tsdemo::SensorValues, OBXPropertyType_Long> objectbox::tsdemo::SensorValues_::id(1);
const obx::Property<objectbox::tsdemo::SensorValues, OBXPropertyType_Date> objectbox::tsdemo::SensorValues_::time(2);
const obx::Property<objectbox::tsdemo::SensorValues, OBXPropertyType_Double> objectbox::tsdemo::SensorValues_::temperatureOutside(3);
const obx::Property<objectbox::tsdemo::SensorValues, OBXPropertyType_Double> objectbox::tsdemo::SensorValues_::temperatureInside(4);
const obx::Property<objectbox::tsdemo::SensorValues, OBXPropertyType_Double> objectbox::tsdemo::SensorValues_::temperatureCpu(5);
const obx::Property<objectbox::tsdemo::SensorValues, OBXPropertyType_Double> objectbox::tsdemo::SensorValues_::loadCpu1(6);
const obx::Property<objectbox::tsdemo::SensorValues, OBXPropertyType_Double> objectbox::tsdemo::SensorValues_::loadCpu2(7);
const obx::Property<objectbox::tsdemo::SensorValues, OBXPropertyType_Double> objectbox::tsdemo::SensorValues_::loadCpu3(8);
const obx::Property<objectbox::tsdemo::SensorValues, OBXPropertyType_Double> objectbox::tsdemo::SensorValues_::loadCpu4(9);

void objectbox::tsdemo::SensorValues_::toFlatBuffer(flatbuffers::FlatBufferBuilder& fbb, const objectbox::tsdemo::SensorValues& object) {
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

objectbox::tsdemo::SensorValues objectbox::tsdemo::SensorValues_::fromFlatBuffer(const void* data, size_t size) {
    objectbox::tsdemo::SensorValues object;
    fromFlatBuffer(data, size, object);
    return object;
}

std::unique_ptr<objectbox::tsdemo::SensorValues> objectbox::tsdemo::SensorValues_::newFromFlatBuffer(const void* data, size_t size) {
    auto object = std::unique_ptr<objectbox::tsdemo::SensorValues>(new objectbox::tsdemo::SensorValues());
    fromFlatBuffer(data, size, *object);
    return object;
}

void objectbox::tsdemo::SensorValues_::fromFlatBuffer(const void* data, size_t, objectbox::tsdemo::SensorValues& outObject) {
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

