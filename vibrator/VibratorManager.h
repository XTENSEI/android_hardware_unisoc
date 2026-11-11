//
// SPDX-FileCopyrightText: The Z2577 Project
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <aidl/android/hardware/vibrator/BnVibratorManager.h>

#include <cstdint>
#include <memory>
#include <vector>

#include "Vibrator.h"

namespace aidl::android::hardware::vibrator {

class VibratorManager : public BnVibratorManager {
  public:
    explicit VibratorManager(std::shared_ptr<Vibrator> vibrator);

    ndk::ScopedAStatus getCapabilities(int32_t* _aidl_return) override;
    ndk::ScopedAStatus getVibratorIds(std::vector<int32_t>* _aidl_return) override;
    ndk::ScopedAStatus getVibrator(int32_t vibratorId,
                                   std::shared_ptr<IVibrator>* _aidl_return) override;
    ndk::ScopedAStatus prepareSynced(const std::vector<int32_t>& vibratorIds) override;
    ndk::ScopedAStatus triggerSynced(const std::shared_ptr<IVibratorCallback>& callback) override;
    ndk::ScopedAStatus cancelSynced() override;

  private:
    std::shared_ptr<Vibrator> vibrator_;
};

}  // namespace aidl::android::hardware::vibrator
