//
// SPDX-FileCopyrightText: The Z2577 Project
// SPDX-License-Identifier: Apache-2.0
//

#include "Memtrack.h"

namespace aidl::android::hardware::memtrack {

namespace {

constexpr char kGpuName[] = "mali0";
constexpr char kUnsupportedMessage[] = "memtrack default implementation";

}  // namespace

ndk::ScopedAStatus Memtrack::getMemory(int32_t, MemtrackType,
                                       std::vector<MemtrackRecord>* _aidl_return) {
    _aidl_return->clear();
    return ndk::ScopedAStatus::fromExceptionCodeWithMessage(EX_UNSUPPORTED_OPERATION,
                                                            kUnsupportedMessage);
}

ndk::ScopedAStatus Memtrack::getGpuDeviceInfo(std::vector<DeviceInfo>* _aidl_return) {
    _aidl_return->clear();
    DeviceInfo device = {.id = 0, .name = kGpuName};
    _aidl_return->push_back(device);
    return ndk::ScopedAStatus::ok();
}

}  // namespace aidl::android::hardware::memtrack
