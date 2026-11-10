//
// SPDX-FileCopyrightText: The Z2577 Project
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <aidl/android/hardware/light/BnLights.h>

#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <thread>

namespace aidl::android::hardware::light {

class Lights : public BnLights {
  public:
    Lights();
    ~Lights() override;

    ndk::ScopedAStatus setLightState(int32_t id, const HwLightState& state) override;
    ndk::ScopedAStatus getLights(std::vector<HwLight>* lights) override;

  private:
    ndk::ScopedAStatus setLightBacklight(const HwLightState& state);
    ndk::ScopedAStatus setLightNotification(const HwLightState& state);
    ndk::ScopedAStatus writeNotification(int32_t color);
    void blink();

    int32_t max_brightness_;

    std::mutex mutex_;
    std::condition_variable wake_;
    int32_t blink_color_ = 0;
    int32_t blink_on_ms_ = 0;
    int32_t blink_off_ms_ = 0;
    uint32_t blink_request_ = 0;
    bool blink_flashing_ = false;
    bool stopping_ = false;
    std::thread blink_thread_;
};

}  // namespace aidl::android::hardware::light
