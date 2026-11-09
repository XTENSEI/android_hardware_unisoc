//
// SPDX-FileCopyrightText: The Z2577 Project
// SPDX-License-Identifier: Apache-2.0
//

#include "Lights.h"

namespace aidl::android::hardware::light {

ndk::ScopedAStatus Lights::setLightState(int32_t id, const HwLightState& state) {
    (void)id;
    (void)state;
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

ndk::ScopedAStatus Lights::getLights(std::vector<HwLight>* lights) {
    lights->clear();
    return ndk::ScopedAStatus::ok();
}

}  // namespace aidl::android::hardware::light
