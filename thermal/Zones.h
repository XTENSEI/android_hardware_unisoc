//
// SPDX-FileCopyrightText: The Z2577 Project
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <aidl/android/hardware/thermal/CoolingDevice.h>
#include <aidl/android/hardware/thermal/Temperature.h>
#include <aidl/android/hardware/thermal/TemperatureThreshold.h>

#include <string>
#include <vector>

namespace aidl::android::hardware::thermal {

class Zones {
  public:
    explicit Zones(std::string root);

    void readTemperatures(std::vector<Temperature>* out) const;
    void readCoolingDevices(std::vector<CoolingDevice>* out) const;
    void readThresholds(std::vector<TemperatureThreshold>* out) const;

  private:
    std::string root_;
};

}  // namespace aidl::android::hardware::thermal
