//
// SPDX-FileCopyrightText: The Z2577 Project
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <aidl/android/hardware/light/BnLights.h>

namespace aidl::android::hardware::light {

class Lights : public BnLights {
  public:
    ndk::ScopedAStatus setLightState(int32_t id, const HwLightState& state) override;
    ndk::ScopedAStatus getLights(std::vector<HwLight>* lights) override;
};

}  // namespace aidl::android::hardware::light
