//
// SPDX-FileCopyrightText: The Z2577 Project
// SPDX-License-Identifier: Apache-2.0
//

#include "Evdev.h"

#include <utility>

#include <fcntl.h>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <unistd.h>

namespace aidl::android::hardware::vibrator {

Evdev::Evdev(std::string path) : path_(std::move(path)) {}

Evdev::~Evdev() {
    if (fd_ < 0) {
        return;
    }
    if (effect_id_ >= 0) {
        ioctl(fd_, EVIOCRMFF, effect_id_);
    }
    close(fd_);
}

bool Evdev::open() {
    if (fd_ >= 0) {
        return true;
    }
    fd_ = ::open(path_.c_str(), O_RDWR | O_CLOEXEC);
    return fd_ >= 0;
}

bool Evdev::writeEvent(uint16_t type, uint16_t code, int32_t value) {
    if (fd_ < 0) {
        return false;
    }
    struct input_event event {};
    event.type = type;
    event.code = code;
    event.value = value;
    return ::write(fd_, &event, sizeof(event)) == static_cast<ssize_t>(sizeof(event));
}

bool Evdev::upload(int32_t duration_ms) {
    struct ff_effect effect {};
    effect.type = FF_RUMBLE;
    effect.id = effect_id_;
    effect.replay.length = duration_ms;
    effect.u.rumble.strong_magnitude = 0xffff;
    effect.u.rumble.weak_magnitude = 0xffff;
    if (ioctl(fd_, EVIOCSFF, &effect) < 0) {
        return false;
    }
    effect_id_ = effect.id;
    return true;
}

bool Evdev::start(int32_t duration_ms) {
    if (!open() || !upload(duration_ms)) {
        return false;
    }
    return writeEvent(EV_FF, effect_id_, 1);
}

void Evdev::stop() {
    if (effect_id_ < 0) {
        return;
    }
    writeEvent(EV_FF, effect_id_, 0);
}

bool Evdev::setGain(int32_t gain) {
    return writeEvent(EV_FF, FF_GAIN, gain);
}

}  // namespace aidl::android::hardware::vibrator
