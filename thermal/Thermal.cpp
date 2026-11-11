//
// SPDX-FileCopyrightText: The Z2577 Project
// SPDX-License-Identifier: Apache-2.0
//

#include "Thermal.h"

#include <algorithm>

namespace aidl::android::hardware::thermal {

namespace {

constexpr char kThermalRoot[] = "/sys/class/thermal";

bool sameCallback(const std::shared_ptr<::ndk::ICInterface>& left,
                  const std::shared_ptr<::ndk::ICInterface>& right) {
    if (left == nullptr || right == nullptr || !left->isRemote() || !right->isRemote()) {
        return left == right;
    }
    return left->asBinder() == right->asBinder();
}

template <typename Callback>
ndk::ScopedAStatus addCallback(const std::shared_ptr<Callback>& callback,
                               std::mutex* mutex, std::vector<std::shared_ptr<Callback>>* list) {
    if (callback == nullptr) {
        return ndk::ScopedAStatus::fromExceptionCodeWithMessage(EX_ILLEGAL_ARGUMENT,
                                                                "Invalid nullptr callback");
    }
    std::lock_guard<std::mutex> lock(*mutex);
    const bool known = std::any_of(list->begin(), list->end(), [&](const auto& entry) {
        return sameCallback(entry, callback);
    });
    if (known) {
        return ndk::ScopedAStatus::fromExceptionCodeWithMessage(EX_ILLEGAL_ARGUMENT,
                                                                "Callback already registered");
    }
    list->push_back(callback);
    return ndk::ScopedAStatus::ok();
}

template <typename Callback>
ndk::ScopedAStatus removeCallback(const std::shared_ptr<Callback>& callback,
                                  std::mutex* mutex,
                                  std::vector<std::shared_ptr<Callback>>* list) {
    if (callback == nullptr) {
        return ndk::ScopedAStatus::fromExceptionCodeWithMessage(EX_ILLEGAL_ARGUMENT,
                                                                "Invalid nullptr callback");
    }
    std::lock_guard<std::mutex> lock(*mutex);
    const auto found = std::find_if(list->begin(), list->end(), [&](const auto& entry) {
        return sameCallback(entry, callback);
    });
    if (found == list->end()) {
        return ndk::ScopedAStatus::fromExceptionCodeWithMessage(EX_ILLEGAL_ARGUMENT,
                                                                "Callback wasn't registered");
    }
    list->erase(found);
    return ndk::ScopedAStatus::ok();
}

}  // namespace

Thermal::Thermal() : zones_(kThermalRoot) {}

ndk::ScopedAStatus Thermal::getTemperatures(std::vector<Temperature>* out_temperatures) {
    zones_.readTemperatures(out_temperatures);
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Thermal::getTemperaturesWithType(
        TemperatureType in_type, std::vector<Temperature>* out_temperatures) {
    std::vector<Temperature> all;
    zones_.readTemperatures(&all);
    out_temperatures->clear();
    for (const Temperature& temperature : all) {
        if (temperature.type == in_type) {
            out_temperatures->push_back(temperature);
        }
    }
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Thermal::getCoolingDevices(std::vector<CoolingDevice>* out_devices) {
    zones_.readCoolingDevices(out_devices);
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Thermal::getCoolingDevicesWithType(CoolingType in_type,
                                                      std::vector<CoolingDevice>* out_devices) {
    std::vector<CoolingDevice> all;
    zones_.readCoolingDevices(&all);
    out_devices->clear();
    for (const CoolingDevice& device : all) {
        if (device.type == in_type) {
            out_devices->push_back(device);
        }
    }
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Thermal::getTemperatureThresholds(
        std::vector<TemperatureThreshold>* out_thresholds) {
    zones_.readThresholds(out_thresholds);
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Thermal::getTemperatureThresholdsWithType(
        TemperatureType in_type, std::vector<TemperatureThreshold>* out_thresholds) {
    std::vector<TemperatureThreshold> all;
    zones_.readThresholds(&all);
    out_thresholds->clear();
    for (const TemperatureThreshold& threshold : all) {
        if (threshold.type == in_type) {
            out_thresholds->push_back(threshold);
        }
    }
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Thermal::registerThermalChangedCallback(
        const std::shared_ptr<IThermalChangedCallback>& in_callback) {
    return addCallback(in_callback, &thermal_mutex_, &thermal_callbacks_);
}

ndk::ScopedAStatus Thermal::registerThermalChangedCallbackWithType(
        const std::shared_ptr<IThermalChangedCallback>& in_callback, TemperatureType) {
    return addCallback(in_callback, &thermal_mutex_, &thermal_callbacks_);
}

ndk::ScopedAStatus Thermal::unregisterThermalChangedCallback(
        const std::shared_ptr<IThermalChangedCallback>& in_callback) {
    return removeCallback(in_callback, &thermal_mutex_, &thermal_callbacks_);
}

ndk::ScopedAStatus Thermal::registerCoolingDeviceChangedCallbackWithType(
        const std::shared_ptr<ICoolingDeviceChangedCallback>& in_callback, CoolingType) {
    return addCallback(in_callback, &cooling_mutex_, &cooling_callbacks_);
}

ndk::ScopedAStatus Thermal::unregisterCoolingDeviceChangedCallback(
        const std::shared_ptr<ICoolingDeviceChangedCallback>& in_callback) {
    return removeCallback(in_callback, &cooling_mutex_, &cooling_callbacks_);
}

ndk::ScopedAStatus Thermal::forecastSkinTemperature(int32_t, float*) {
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

}  // namespace aidl::android::hardware::thermal
