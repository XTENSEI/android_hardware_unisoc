//
// SPDX-FileCopyrightText: The Z2577 Project
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <aidl/android/hardware/light/BnLights.h>

namespace aidl::android::hardware::light {

class Lights : public BnLights {
  public:
    Lights();

    ndk::ScopedAStatus setLightState(int32_t id, const HwLightState& state) override;
    ndk::ScopedAStatus getLights(std::vector<HwLight>* lights) override;

  private:
    ndk::ScopedAStatus setLightBacklight(const HwLightState& state);
    ndk::ScopedAStatus setLightNotification(const HwLightState& state);

    int32_t max_brightness_;
};

}  // namespace aidl::android::hardware::light
