//
// SPDX-FileCopyrightText: The Z2577 Project
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <aidl/android/hardware/thermal/BnThermal.h>

#include <memory>
#include <mutex>
#include <vector>

#include "Zones.h"

namespace aidl::android::hardware::thermal {

class Thermal : public BnThermal {
  public:
    Thermal();

    ndk::ScopedAStatus getCoolingDevices(std::vector<CoolingDevice>* out_devices) override;
    ndk::ScopedAStatus getCoolingDevicesWithType(CoolingType in_type,
                                                 std::vector<CoolingDevice>* out_devices) override;
    ndk::ScopedAStatus getTemperatures(std::vector<Temperature>* out_temperatures) override;
    ndk::ScopedAStatus getTemperaturesWithType(
            TemperatureType in_type, std::vector<Temperature>* out_temperatures) override;
    ndk::ScopedAStatus getTemperatureThresholds(
            std::vector<TemperatureThreshold>* out_thresholds) override;
    ndk::ScopedAStatus getTemperatureThresholdsWithType(
            TemperatureType in_type, std::vector<TemperatureThreshold>* out_thresholds) override;
    ndk::ScopedAStatus registerThermalChangedCallback(
            const std::shared_ptr<IThermalChangedCallback>& in_callback) override;
    ndk::ScopedAStatus registerThermalChangedCallbackWithType(
            const std::shared_ptr<IThermalChangedCallback>& in_callback,
            TemperatureType in_type) override;
    ndk::ScopedAStatus unregisterThermalChangedCallback(
            const std::shared_ptr<IThermalChangedCallback>& in_callback) override;
    ndk::ScopedAStatus registerCoolingDeviceChangedCallbackWithType(
            const std::shared_ptr<ICoolingDeviceChangedCallback>& in_callback,
            CoolingType in_type) override;
    ndk::ScopedAStatus unregisterCoolingDeviceChangedCallback(
            const std::shared_ptr<ICoolingDeviceChangedCallback>& in_callback) override;
    ndk::ScopedAStatus forecastSkinTemperature(int32_t forecastSeconds,
                                               float* _aidl_return) override;

  private:
    Zones zones_;

    std::mutex thermal_mutex_;
    std::vector<std::shared_ptr<IThermalChangedCallback>> thermal_callbacks_;
    std::mutex cooling_mutex_;
    std::vector<std::shared_ptr<ICoolingDeviceChangedCallback>> cooling_callbacks_;
};

}  // namespace aidl::android::hardware::thermal
