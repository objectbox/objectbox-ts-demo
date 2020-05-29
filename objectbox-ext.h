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

class NotFoundException : public std::exception {};

/// Transactions can be started in read (only) or write mode.
enum class TxMode { READ, WRITE };

namespace {
#define OBJECTBOX_VERIFY_ARGUMENT(c) \
    ((c) ? (void) (0) : throw std::invalid_argument(std::string("Argument validation failed: #c")))

#define OBJECTBOX_VERIFY_STATE(c) \
    ((c) ? (void) (0) : throw std::runtime_error(std::string("State condition failed: #c")))

[[noreturn]] void throwLastError() {
    throw Exception(obx_last_error_message(), obx_last_error_code());
}

void checkErrOrThrow(obx_err err) {
    if (err == OBX_SUCCESS) return;
    if (err == OBX_NOT_FOUND) throw NotFoundException();
    throwLastError();
}
void checkPtrOrThrow(void* ptr, const std::string& context) {
    if (!ptr) throw Exception(context + ": " + obx_last_error_message(), obx_last_error_code());
}

/// Internal RAII transaction wrapper
class Transaction {
    TxMode mode_;
    OBX_txn* txn_;
    bool successful_ = true;

public:
    explicit Transaction(TxMode mode, OBX_store* cStore)
        : mode_(mode), txn_(mode == TxMode::WRITE ? obx_txn_write(cStore) : obx_txn_read(cStore)) {
        checkPtrOrThrow(txn_, "can't start transaction");
    }
    virtual ~Transaction() {
        obx_err err = 0;
        if (mode_ == TxMode::WRITE) {
            err = obx_txn_mark_success(txn_, successful_);
        }
        obx_err err2 = obx_txn_close(txn_);
        // TODO is it good idea to (potentially) throw here?
        checkErrOrThrow(err);
        checkErrOrThrow(err2);
    }
    void fail() { successful_ = false; }
};
}  // namespace

template <class EntityT>
class Box;

// TODO maybe Box vending function?
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
        checkPtrOrThrow(opt, "can't create store options");
        obx_opt_max_db_size_in_kb(opt, 10 * 1024 * 1024);
        obx_opt_model(opt, model);
        checkPtrOrThrow(cStore_ = obx_store_open(opt), "can't open store");
    }

    virtual ~Store() { obx_store_close(cStore_); }

    /// Executes a given function inside a transaction.
    template <typename FN>
    auto runInTransaction(TxMode mode, FN fn) -> decltype(fn()) {
        Transaction txn(mode, cStore_);
        try {
            return fn();
        } catch (...) {
            txn.fail();
            throw;
        }
    }
};

namespace {
thread_local flatbuffers::FlatBufferBuilder fbb;
}

template <class EntityT>
class Box {
    OBX_box* cBox_;
    Store& store_;

public:
    explicit Box(Store& store) : store_(store), cBox_(obx_box(store.cStore_, EntityT::entityId())) {
        checkPtrOrThrow(cBox_, "can't create box");
    }

    // TODO return type so that we can support c++11
    std::unique_ptr<EntityT> get(obx_id id) {
        Transaction txn(TxMode::READ, store_.cStore_);
        void* data = nullptr;
        size_t size = 0;
        checkErrOrThrow(obx_box_get(cBox_, id, &data, &size));
        return EntityT::newFromFlatBuffer(data, size);
    }

    obx_id put(const EntityT& object) {
        Transaction txn(TxMode::WRITE, store_.cStore_);
        return put(txn, object);
    }

    void put(const std::vector<EntityT>& objects) {
        Transaction txn(TxMode::WRITE, store_.cStore_);
        for (auto& object : objects) {
            put(txn, object);
        }
    }

    uint64_t count() {
        uint64_t count;
        checkErrOrThrow(obx_box_count(cBox_, 0, &count));
        return count;
    }

private:
    template <typename T>
    obx_id put(Transaction& txn, const T& object) {
        fbb.Clear();
        EntityT::toFlatBuffer(fbb, object);
        obx_id id =
            obx_box_put_object(cBox_, fbb.GetBufferPointer(), fbb.GetSize(), OBXPutMode_PUT);
        if (id == 0) throwLastError();
        return id;
    }
};

}  // namespace objectbox