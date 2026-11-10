//
// SPDX-FileCopyrightText: The Z2577 Project
// SPDX-License-Identifier: Apache-2.0
//

#include "Lights.h"

#include <cerrno>
#include <chrono>
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

Lights::Lights() : max_brightness_(readMaxBrightness()), blink_thread_(&Lights::blink, this) {}

Lights::~Lights() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        stopping_ = true;
    }
    wake_.notify_all();
    if (blink_thread_.joinable()) {
        blink_thread_.join();
    }
}

ndk::ScopedAStatus Lights::setLightBacklight(const HwLightState& state) {
    int32_t brightness = state.color & 0xff;
    int32_t value = (brightness * max_brightness_ + 127) / 255;
    return writeBrightness(std::string(kBacklightDir) + "brightness", value);
}

ndk::ScopedAStatus Lights::writeNotification(int32_t color) {
    ndk::ScopedAStatus status =
            writeBrightness(std::string(kLedDir) + "red/brightness", (color >> 16) & 0xff);
    if (!status.isOk()) {
        return status;
    }
    status = writeBrightness(std::string(kLedDir) + "green/brightness", (color >> 8) & 0xff);
    if (!status.isOk()) {
        return status;
    }
    return writeBrightness(std::string(kLedDir) + "blue/brightness", color & 0xff);
}

ndk::ScopedAStatus Lights::setLightNotification(const HwLightState& state) {
    const bool flashing =
            state.flashMode != FlashMode::NONE && state.flashOnMs > 0 && state.flashOffMs > 0;

    std::lock_guard<std::mutex> lock(mutex_);
    blink_color_ = state.color & 0xffffff;
    blink_flashing_ = flashing;
    blink_on_ms_ = state.flashOnMs;
    blink_off_ms_ = state.flashOffMs;
    ++blink_request_;

    if (!flashing) {
        return writeNotification(blink_color_);
    }
    wake_.notify_one();
    return ndk::ScopedAStatus::ok();
}

void Lights::blink() {
    std::unique_lock<std::mutex> lock(mutex_);
    uint32_t seen = 0;

    while (true) {
        wake_.wait(lock, [this, &seen] { return stopping_ || seen != blink_request_; });
        if (stopping_) {
            return;
        }
        seen = blink_request_;
        if (!blink_flashing_) {
            continue;
        }

        while (seen == blink_request_) {
            writeNotification(blink_color_);
            if (wake_.wait_for(lock, std::chrono::milliseconds(blink_on_ms_),
                               [this, seen] { return stopping_ || seen != blink_request_; })) {
                break;
            }
            writeNotification(0);
            if (wake_.wait_for(lock, std::chrono::milliseconds(blink_off_ms_),
                               [this, seen] { return stopping_ || seen != blink_request_; })) {
                break;
            }
        }
        if (stopping_) {
            return;
        }
    }
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
