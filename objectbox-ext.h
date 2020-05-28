/*
 * Copyright 2018-2020 ObjectBox Ltd. All rights reserved.
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

#pragma once

#include "flatbuffers/flatbuffers.h"
#include "objectbox.h"

namespace objectbox {
class Exception : public std::runtime_error {
    const int code_;  // TODO make code private & provide a getter?
public:
    explicit Exception(const std::string& text, int code = 0) : runtime_error(text), code_(code) {}
    explicit Exception(const char* text, int code = 0) : runtime_error(text), code_(code) {}
    int code() const { return code_; }
};

namespace {
#define OBJECTBOX_VERIFY_ARGUMENT(c) \
    ((c) ? (void) (0) : throw std::invalid_argument(std::string("Argument validation failed: #c")))

#define OBJECTBOX_VERIFY_STATE(c) \
    ((c) ? (void) (0) : throw std::runtime_error(std::string("State condition failed: #c")))

void checkPtrOrThrow(void* ptr) {
    if (ptr) return;
    throw Exception(obx_last_error_message(), obx_last_error_code());
}
}  // namespace

template <class EntityT>
class Box;

class Store {
    OBX_store* cStore_;

    template <class EntityT>
    friend class Box;

public:
    // TODO options, maybe a builder with a fluent interface?
    explicit Store(OBX_model* model) {
        OBJECTBOX_VERIFY_ARGUMENT(model != nullptr);

        {  // check model error explicitly or it may be swallowed later
            obx_err err = obx_model_error_code(model);
            if (err) {
                const char* msg = obx_model_error_message(model);
                obx_model_free(model);
                throw Exception(msg, err);
            }
        }

        OBX_store_options* opt = obx_opt();
        checkPtrOrThrow(opt);
        obx_opt_max_db_size_in_kb(opt, 10 * 1024 * 1024);
        obx_opt_model(opt, model);
        checkPtrOrThrow(cStore_ = obx_store_open(opt));
    }
};

template <class EntityT>
class EntityBinding {
    /// Write given object to the FlatBufferBuilder
    static void toFlatBuffer(flatbuffers::FlatBufferBuilder& fbb, const EntityT& object);

    /// Read an object from a valid FlatBuffer
    static EntityT fromFlatBuffer(const void* data, size_t size);

    /// Read an object from a valid FlatBuffer
    static void fromFlatBuffer(const void* data, size_t size, EntityT& outObject);
};

template <class EntityT>
class Box {
    OBX_box* cBox_;

public:
    // TODO how to do the initialization nicely, e.g. store.boxFor(entity)?
    Box(Store& store, obx_schema_id entityId) {
        OBJECTBOX_VERIFY_ARGUMENT(entityId > 0);
        checkPtrOrThrow(cBox_ = obx_box(store.cStore_, entityId));
    }
    void get() {  // EntityBinding<EntityT>::fromFlatBuffer(data, data.size);
    }
};

}  // namespace objectbox