#include <cstdlib>

#include "objectbox-model.h"
#include "objectbox.h"

obx_err printError(const char* hint = nullptr) {
    if (hint) printf("Error: %s\n", hint);
    printf("Last error: %d, %d (%s)\n", obx_last_error_code(), obx_last_error_secondary(),
           obx_last_error_message());
    return obx_last_error_code();
}

[[noreturn]] void printErrorAndExit(const char* hint = nullptr) { exit(printError(hint)); }

int main(int argc, char* args[]) {
    printf("ObjectBox TS demo using version %s, core version: %s\n", obx_version_string(),
           obx_version_core_string());

    OBX_model* model = create_obx_model();
    if (!model) printErrorAndExit("model");

    OBX_store_options* opt = obx_opt();
    obx_opt_model(opt, model);
    OBX_store* store = obx_store_open(opt);
    if (!store) printErrorAndExit("store");

    printf("ObjectBox store opened");

    if (obx_store_close(store)) printErrorAndExit("store close");

    return 0;
}
