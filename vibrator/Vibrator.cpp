//
// SPDX-FileCopyrightText: The Z2577 Project
// SPDX-License-Identifier: Apache-2.0
//

#include "Vibrator.h"

#include <cerrno>

#include <android-base/logging.h>

namespace aidl::android::hardware::vibrator {

namespace {

constexpr char kEvdevPath[] = "/dev/input/event1";

}  // namespace

Vibrator::Vibrator() : device_(kEvdevPath) {}

ndk::ScopedAStatus Vibrator::getCapabilities(int32_t* _aidl_return) {
    *_aidl_return = CAP_AMPLITUDE_CONTROL;
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Vibrator::off() {
    device_.stop();
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Vibrator::on(int32_t timeoutMs, const std::shared_ptr<IVibratorCallback>&) {
    if (timeoutMs <= 0) {
        device_.stop();
        return ndk::ScopedAStatus::ok();
    }
    if (!device_.start(timeoutMs)) {
        LOG(ERROR) << "Cannot vibrate for " << timeoutMs << " ms";
        return ndk::ScopedAStatus::fromServiceSpecificError(errno != 0 ? errno : EIO);
    }
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Vibrator::perform(Effect, EffectStrength,
                                     const std::shared_ptr<IVibratorCallback>&, int32_t*) {
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

ndk::ScopedAStatus Vibrator::getSupportedEffects(std::vector<Effect>* _aidl_return) {
    _aidl_return->clear();
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Vibrator::setAmplitude(float amplitude) {
    int32_t magnitude = 0;
    if (amplitude > 1.0f) {
        magnitude = 0xffff;
    } else if (amplitude > 0.0f) {
        magnitude = static_cast<int32_t>(amplitude * 0xffff);
    }
    device_.setMagnitude(magnitude);
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Vibrator::setExternalControl(bool) {
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

ndk::ScopedAStatus Vibrator::getCompositionDelayMax(int32_t* _aidl_return) {
    *_aidl_return = 0;
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Vibrator::getCompositionSizeMax(int32_t* _aidl_return) {
    *_aidl_return = 0;
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Vibrator::getSupportedPrimitives(std::vector<CompositePrimitive>* _aidl_return) {
    _aidl_return->clear();
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Vibrator::getPrimitiveDuration(CompositePrimitive, int32_t* _aidl_return) {
    *_aidl_return = 0;
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Vibrator::compose(const std::vector<CompositeEffect>&,
                                     const std::shared_ptr<IVibratorCallback>&) {
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

ndk::ScopedAStatus Vibrator::getSupportedAlwaysOnEffects(std::vector<Effect>* _aidl_return) {
    _aidl_return->clear();
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Vibrator::alwaysOnEnable(int32_t, Effect, EffectStrength) {
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

ndk::ScopedAStatus Vibrator::alwaysOnDisable(int32_t) {
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

ndk::ScopedAStatus Vibrator::getResonantFrequency(float* _aidl_return) {
    *_aidl_return = 0.0f;
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Vibrator::getQFactor(float* _aidl_return) {
    *_aidl_return = 0.0f;
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Vibrator::getFrequencyResolution(float* _aidl_return) {
    *_aidl_return = 0.0f;
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Vibrator::getFrequencyMinimum(float* _aidl_return) {
    *_aidl_return = 0.0f;
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Vibrator::getBandwidthAmplitudeMap(std::vector<float>* _aidl_return) {
    _aidl_return->clear();
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Vibrator::getPwlePrimitiveDurationMax(int32_t* _aidl_return) {
    *_aidl_return = 0;
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Vibrator::getPwleCompositionSizeMax(int32_t* _aidl_return) {
    *_aidl_return = 0;
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Vibrator::getSupportedBraking(std::vector<Braking>* _aidl_return) {
    _aidl_return->clear();
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Vibrator::composePwle(const std::vector<PrimitivePwle>&,
                                         const std::shared_ptr<IVibratorCallback>&) {
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

}  // namespace aidl::android::hardware::vibrator
