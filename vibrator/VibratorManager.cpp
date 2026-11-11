//
// SPDX-FileCopyrightText: The Z2577 Project
// SPDX-License-Identifier: Apache-2.0
//

#include "VibratorManager.h"

#include <utility>

namespace aidl::android::hardware::vibrator {

namespace {

constexpr int32_t kVibratorId = 0;

}  // namespace

VibratorManager::VibratorManager(std::shared_ptr<Vibrator> vibrator)
    : vibrator_(std::move(vibrator)) {}

ndk::ScopedAStatus VibratorManager::getCapabilities(int32_t* _aidl_return) {
    *_aidl_return = 0;
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus VibratorManager::getVibratorIds(std::vector<int32_t>* _aidl_return) {
    *_aidl_return = {kVibratorId};
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus VibratorManager::getVibrator(int32_t vibratorId,
                                                std::shared_ptr<IVibrator>* _aidl_return) {
    if (vibratorId != kVibratorId) {
        return ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_ARGUMENT);
    }
    *_aidl_return = vibrator_;
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus VibratorManager::prepareSynced(const std::vector<int32_t>&) {
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

ndk::ScopedAStatus VibratorManager::triggerSynced(const std::shared_ptr<IVibratorCallback>&) {
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

ndk::ScopedAStatus VibratorManager::cancelSynced() {
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

}  // namespace aidl::android::hardware::vibrator
