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

#include "VibratorManager.h"

using aidl::android::hardware::vibrator::Vibrator;
using aidl::android::hardware::vibrator::VibratorManager;

int main() {
    ABinderProcess_setThreadPoolMaxThreadCount(0);

    std::shared_ptr<Vibrator> vibrator = ndk::SharedRefBase::make<Vibrator>();
    std::shared_ptr<VibratorManager> manager = ndk::SharedRefBase::make<VibratorManager>(vibrator);

    const std::string vibratorInstance = std::string(Vibrator::descriptor) + "/default";
    binder_status_t status =
            AServiceManager_addService(vibrator->asBinder().get(), vibratorInstance.c_str());
    CHECK_EQ(status, STATUS_OK) << "Cannot register " << vibratorInstance;

    const std::string managerInstance = std::string(VibratorManager::descriptor) + "/default";
    status = AServiceManager_addService(manager->asBinder().get(), managerInstance.c_str());
    CHECK_EQ(status, STATUS_OK) << "Cannot register " << managerInstance;

    ABinderProcess_joinThreadPool();
    return EXIT_FAILURE;
}
