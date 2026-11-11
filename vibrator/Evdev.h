//
// SPDX-FileCopyrightText: The Z2577 Project
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <cstdint>
#include <string>

namespace aidl::android::hardware::vibrator {

class Evdev {
  public:
    explicit Evdev(std::string path);
    ~Evdev();

    bool start(int32_t duration_ms);
    void stop();
    bool setGain(int32_t gain);

  private:
    bool open();
    bool upload(int32_t duration_ms);
    bool writeEvent(uint16_t type, uint16_t code, int32_t value);

    std::string path_;
    int32_t fd_ = -1;
    int32_t effect_id_ = -1;
};

}  // namespace aidl::android::hardware::vibrator
