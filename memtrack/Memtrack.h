//
// SPDX-FileCopyrightText: The Z2577 Project
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <aidl/android/hardware/memtrack/BnMemtrack.h>
#include <aidl/android/hardware/memtrack/DeviceInfo.h>
#include <aidl/android/hardware/memtrack/MemtrackRecord.h>
#include <aidl/android/hardware/memtrack/MemtrackType.h>

#include <vector>

namespace aidl::android::hardware::memtrack {

class Memtrack : public BnMemtrack {
  public:
    ndk::ScopedAStatus getMemory(int32_t pid, MemtrackType type,
                                 std::vector<MemtrackRecord>* _aidl_return) override;
    ndk::ScopedAStatus getGpuDeviceInfo(std::vector<DeviceInfo>* _aidl_return) override;
};

}  // namespace aidl::android::hardware::memtrack
