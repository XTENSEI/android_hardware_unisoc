//
// SPDX-FileCopyrightText: The Z2577 Project
// SPDX-License-Identifier: Apache-2.0
//

#include "Zones.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <limits>
#include <system_error>
#include <utility>

namespace aidl::android::hardware::thermal {

namespace {

namespace fs = std::filesystem;

constexpr int32_t kPassiveFirst = static_cast<int32_t>(ThrottlingSeverity::LIGHT);
constexpr int32_t kPassiveLast = static_cast<int32_t>(ThrottlingSeverity::SEVERE);
constexpr int32_t kShutdown = static_cast<int32_t>(ThrottlingSeverity::SHUTDOWN);
constexpr int32_t kSeverityCount = static_cast<int32_t>(ThrottlingSeverity::SHUTDOWN) + 1;
constexpr float kMillidegrees = 1000.0f;
constexpr char kZonePrefix[] = "thermal_zone";
constexpr char kCoolingPrefix[] = "cooling_device";

std::string readFile(const fs::path& path) {
    std::ifstream file(path);
    std::string line;
    if (!std::getline(file, line)) {
        return {};
    }
    return line;
}

bool readCelsius(const fs::path& path, float* out) {
    const std::string text = readFile(path);
    if (text.empty()) {
        return false;
    }
    char* end = nullptr;
    const float millis = std::strtof(text.c_str(), &end);
    if (end == text.c_str()) {
        return false;
    }
    *out = millis / kMillidegrees;
    return true;
}

int32_t suffixOf(const fs::path& path, const char* prefix) {
    const std::string name = path.filename().string();
    return std::atoi(name.c_str() + std::strlen(prefix));
}

std::vector<fs::path> children(const fs::path& root, const char* prefix) {
    std::vector<fs::path> found;
    std::error_code error;
    for (const auto& entry : fs::directory_iterator(root, error)) {
        const std::string name = entry.path().filename().string();
        if (name.compare(0, std::strlen(prefix), prefix) == 0) {
            found.push_back(entry.path());
        }
    }
    std::sort(found.begin(), found.end(), [&](const fs::path& left, const fs::path& right) {
        return suffixOf(left, prefix) < suffixOf(right, prefix);
    });
    return found;
}

TemperatureType temperatureTypeOf(const std::string& name) {
    if (name.find("cpu") != std::string::npos) {
        return TemperatureType::CPU;
    }
    if (name.find("gpu") != std::string::npos) {
        return TemperatureType::GPU;
    }
    if (name.find("battery") != std::string::npos) {
        return TemperatureType::BATTERY;
    }
    if (name.find("skin") != std::string::npos) {
        return TemperatureType::SKIN;
    }
    if (name.find("pa-") != std::string::npos) {
        return TemperatureType::POWER_AMPLIFIER;
    }
    if (name.find("soc") != std::string::npos) {
        return TemperatureType::SOC;
    }
    return TemperatureType::UNKNOWN;
}

CoolingType coolingTypeOf(const std::string& name) {
    if (name.find("gpu") != std::string::npos) {
        return CoolingType::GPU;
    }
    if (name.find("cpu") != std::string::npos) {
        return CoolingType::CPU;
    }
    return CoolingType::COMPONENT;
}

std::vector<float> nanThresholds() {
    return std::vector<float>(kSeverityCount, std::numeric_limits<float>::quiet_NaN());
}

std::vector<float> hotThresholdsOf(const fs::path& zone) {
    std::vector<float> thresholds = nanThresholds();
    std::vector<float> passive;
    std::error_code error;

    for (const auto& entry : fs::directory_iterator(zone, error)) {
        const std::string name = entry.path().filename().string();
        const std::string suffix = name.substr(name.rfind('_') + 1);
        if (suffix != "temp" || name.find("trip_point_") != 0) {
            continue;
        }
        float celsius = 0.0f;
        if (!readCelsius(entry.path(), &celsius)) {
            continue;
        }
        const fs::path type_path = zone / (name.substr(0, name.size() - 5) + "_type");
        const std::string type = readFile(type_path);
        if (type == "critical") {
            thresholds[kShutdown] = celsius;
        } else if (type == "passive" || type == "hot") {
            passive.push_back(celsius);
        }
    }

    std::sort(passive.begin(), passive.end());
    int32_t level = kPassiveFirst;
    for (const float celsius : passive) {
        if (level > kPassiveLast) {
            break;
        }
        thresholds[level++] = celsius;
    }
    return thresholds;
}

ThrottlingSeverity severityOf(float value, const std::vector<float>& thresholds) {
    for (int32_t level = kSeverityCount - 1; level >= 1; --level) {
        if (!std::isnan(thresholds[level]) && value >= thresholds[level]) {
            return static_cast<ThrottlingSeverity>(level);
        }
    }
    return ThrottlingSeverity::NONE;
}

struct Reading {
    std::string name;
    float value;
    std::vector<float> hot;
    std::vector<float> cold;
};

std::vector<Reading> collect(const fs::path& root) {
    std::vector<Reading> readings;
    for (const auto& zone : children(root, kZonePrefix)) {
        const std::string name = readFile(zone / "type");
        float value = 0.0f;
        if (name.empty() || !readCelsius(zone / "temp", &value)) {
            continue;
        }
        readings.push_back(Reading{name, value, hotThresholdsOf(zone), nanThresholds()});
    }
    return readings;
}

}  // namespace

Zones::Zones(std::string root) : root_(std::move(root)) {}

void Zones::readTemperatures(std::vector<Temperature>* out) const {
    out->clear();
    for (const Reading& reading : collect(root_)) {
        Temperature temperature = {};
        temperature.type = temperatureTypeOf(reading.name);
        temperature.name = reading.name;
        temperature.value = reading.value;
        temperature.throttlingStatus = severityOf(reading.value, reading.hot);
        out->push_back(temperature);
    }
}

void Zones::readCoolingDevices(std::vector<CoolingDevice>* out) const {
    out->clear();
    for (const auto& device : children(root_, kCoolingPrefix)) {
        const std::string name = readFile(device / "type");
        const std::string state = readFile(device / "cur_state");
        if (name.empty() || state.empty()) {
            continue;
        }
        CoolingDevice cooling = {};
        cooling.type = coolingTypeOf(name);
        cooling.name = name;
        cooling.value = std::atol(state.c_str());
        out->push_back(cooling);
    }
}

void Zones::readThresholds(std::vector<TemperatureThreshold>* out) const {
    out->clear();
    for (const Reading& reading : collect(root_)) {
        TemperatureThreshold threshold = {};
        threshold.type = temperatureTypeOf(reading.name);
        threshold.name = reading.name;
        threshold.hotThrottlingThresholds = reading.hot;
        threshold.coldThrottlingThresholds = reading.cold;
        out->push_back(threshold);
    }
}

}  // namespace aidl::android::hardware::thermal
