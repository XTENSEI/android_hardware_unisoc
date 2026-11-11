//
// SPDX-FileCopyrightText: The Z2577 Project
// SPDX-License-Identifier: Apache-2.0
//

#include <cstdlib>
#include <memory>
#include <string>

#include <android-base/logging.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>

#include "Thermal.h"

using aidl::android::hardware::thermal::Thermal;

int main() {
    ABinderProcess_setThreadPoolMaxThreadCount(0);

    std::shared_ptr<Thermal> thermal = ndk::SharedRefBase::make<Thermal>();
    const std::string instance = std::string(Thermal::descriptor) + "/default";
    binder_status_t status =
            AServiceManager_addService(thermal->asBinder().get(), instance.c_str());
    CHECK_EQ(status, STATUS_OK) << "Cannot register " << instance;

    ABinderProcess_joinThreadPool();
    return EXIT_FAILURE;
}
