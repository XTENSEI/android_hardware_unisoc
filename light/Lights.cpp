//
// SPDX-FileCopyrightText: The Z2577 Project
// SPDX-License-Identifier: Apache-2.0
//

#include "Lights.h"

#include <cerrno>
#include <cstdlib>
#include <string>

#include <android-base/file.h>
#include <android-base/logging.h>

namespace aidl::android::hardware::light {

namespace {

constexpr int32_t kBacklightId = 0;
constexpr int32_t kNotificationId = 1;
constexpr char kBacklightDir[] = "/sys/class/backlight/sprd_backlight/";
constexpr char kLedDir[] = "/sys/class/leds/sc27xx:";

int32_t readMaxBrightness() {
    std::string value;
    if (!::android::base::ReadFileToString(std::string(kBacklightDir) + "max_brightness", &value)) {
        LOG(ERROR) << "Cannot read the backlight ceiling";
        return 255;
    }
    int32_t max = std::atoi(value.c_str());
    return max > 0 ? max : 255;
}

ndk::ScopedAStatus writeBrightness(const std::string& path, int32_t value) {
    if (!::android::base::WriteStringToFile(std::to_string(value), path)) {
        LOG(ERROR) << "Cannot write " << value << " to " << path;
        return ndk::ScopedAStatus::fromServiceSpecificError(errno != 0 ? errno : EIO);
    }
    return ndk::ScopedAStatus::ok();
}

}  // namespace

Lights::Lights() : max_brightness_(readMaxBrightness()) {}

ndk::ScopedAStatus Lights::setLightBacklight(const HwLightState& state) {
    int32_t brightness = state.color & 0xff;
    int32_t value = (brightness * max_brightness_ + 127) / 255;
    return writeBrightness(std::string(kBacklightDir) + "brightness", value);
}

ndk::ScopedAStatus Lights::setLightNotification(const HwLightState& state) {
    ndk::ScopedAStatus status =
            writeBrightness(std::string(kLedDir) + "red/brightness", (state.color >> 16) & 0xff);
    if (!status.isOk()) {
        return status;
    }
    status = writeBrightness(std::string(kLedDir) + "green/brightness", (state.color >> 8) & 0xff);
    if (!status.isOk()) {
        return status;
    }
    return writeBrightness(std::string(kLedDir) + "blue/brightness", state.color & 0xff);
}

ndk::ScopedAStatus Lights::setLightState(int32_t id, const HwLightState& state) {
    switch (id) {
        case kBacklightId:
            return setLightBacklight(state);
        case kNotificationId:
            return setLightNotification(state);
        default:
            return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
    }
}

ndk::ScopedAStatus Lights::getLights(std::vector<HwLight>* lights) {
    lights->clear();
    lights->push_back(HwLight{kBacklightId, 0, LightType::BACKLIGHT});
    lights->push_back(HwLight{kNotificationId, 0, LightType::NOTIFICATIONS});
    return ndk::ScopedAStatus::ok();
}

}  // namespace aidl::android::hardware::light
