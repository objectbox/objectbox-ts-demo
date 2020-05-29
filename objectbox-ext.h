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

#include <thread>

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

class NotFoundException : public Exception {
public:
    NotFoundException() : Exception("not found") {}
};

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
}  // namespace

template <class EntityT>
class Box;

class Transaction;

// TODO maybe Box vending function?
class Store {
    OBX_store* cStore_;

    template <class EntityT>
    friend class Box;

    friend class Transaction;

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
};

/// Provides RAII wrapper for an active database transaction on the current thread (do not use
/// across threads). A Transaction object is considered a "top level transaction" if it is the first
/// one on the call stack in the thread. If the thread already has an ongoing Transaction,
/// additional Transaction instances are considered "inner transactions".
///
/// The top level transaction defines the actual transaction scope on the DB level.
/// Internally, the top level Transaction object manages (creates and destroys) a Transaction
/// object. Inner transactions use the Transaction object of the top level Transaction.
///
/// For write transactions, the top level call to success() actually commits the underlying
/// Transaction. If inner transactions are spawned, all of them must call success() in order for the
/// top level transaction to be successful and actually commit.
class Transaction {
    OBX_txn* cTxn_;
    TxMode mode_;
    const std::thread::id threadId_;
    bool successful_ = false;
    bool closed_ = false;

public:
    explicit Transaction(TxMode mode, Store& store)
        : mode_(mode),
          cTxn_(mode == TxMode::WRITE ? obx_txn_write(store.cStore_) : obx_txn_read(store.cStore_)),
          threadId_(std::this_thread::get_id()) {
        checkPtrOrThrow(cTxn_, "can't start transaction");
    }

    /// Note: If you rely on committing in the destructor, you must ensure no uncaught exception
    /// occurs after markSuccess() was called! See markSuccess() for details. This does not
    /// apply if you use the success() instead.
    virtual ~Transaction() noexcept(false) { close(); };

    /// Delete because the default copy constructor can break things (i.e. a Transaction can not
    /// be copied).
    Transaction(Transaction&) = delete;

    /// "Finishes" this write transaction successfully; performs a commit if this is the top level
    /// transaction and all inner transactions (if any) were also successful. This object will also
    /// be "closed".
    /// @throws Exception if this is not a write TX or it was closed before (e.g. via success()).
    void success() {
        try {
            if (mode_ != TxMode::WRITE) {
                throw Exception("Cannot set success flag on a non-write TX");
            } else if (closed_) {
                throw Exception("Cannot set success flag on a closed TX");
            }
            successful_ = true;
        } catch (...) {
            close();
            throw;
        }
        close();
    }

private:
    /// Closing a top level Transaction marked as successful performs a commit (write TX only and
    /// with all inner TX successful). It's OK to call this method multiple time; additional calls
    /// will have no effect.
    void close() {
        if (closed_) return;
        closed_ = true;

        if (mode_ == TxMode::WRITE && successful_) {
            checkErrOrThrow(obx_txn_success(cTxn_));
            return;  // closed already
        }
        checkErrOrThrow(obx_txn_close(cTxn_));
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

    /// Read an object from the database
    /// @throws
    /// @return an object pointer or nullptr if the object doesn't exist
    std::unique_ptr<EntityT> get(obx_id id) {
        Transaction txn(TxMode::READ, store_);
        void* data = nullptr;
        size_t size = 0;
        obx_err err = obx_box_get(cBox_, id, &data, &size);
        if (err == OBX_NOT_FOUND) return nullptr;
        checkErrOrThrow(err);
        return EntityT::newFromFlatBuffer(data, size);
    }

//    // TODO
//    bool get(obx_id id, EntityT& outObject) {
//        Transaction txn(TxMode::READ, store_);
//        void* data = nullptr;
//        size_t size = 0;
//        obx_err err = obx_box_get(cBox_, id, &data, &size);
//        if (err == OBX_NOT_FOUND) return nullptr;
//        return EntityT::newFromFlatBuffer(data, size);
//    }

    obx_id put(EntityT& object) {
        Transaction txn(TxMode::WRITE, store_);
        obx_id id = put(txn, object);
        txn.success();
        return id;
    }

    void put(std::vector<EntityT>& objects, std::vector<obx_id>* outIds = nullptr) {  // TODO
        Transaction txn(TxMode::WRITE, store_);
        for (auto& object : objects) {
            put(txn, object);
        }
        txn.success();
    }

    uint64_t count(uint64_t limit = 0) {  // TODO
        uint64_t count;
        checkErrOrThrow(obx_box_count(cBox_, 0, &count));
        return count;
    }

private:
    obx_id put(Transaction& txn, EntityT& object) {
        fbb.Clear();
        EntityT::toFlatBuffer(fbb, object);
        obx_id id =
            obx_box_put_object(cBox_, fbb.GetBufferPointer(), fbb.GetSize(), OBXPutMode_PUT);
        if (id == 0) throwLastError();
        object.setObjectId(id);
        return id;
    }
};

}  // namespace objectbox