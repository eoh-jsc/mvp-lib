#ifndef INC_ERA_API_MBED_HPP_
#define INC_ERA_API_MBED_HPP_

#include <mbed.h>
#include <ERa/ERaApi.hpp>

using namespace mbed;

inline
static uint16_t analogReadMbed(uint8_t pin) {
    AnalogIn p((PinName)pin);
    return uint16_t(p.read() * 4095);
}

inline
static void pwmWrite(uint8_t pin, uint8_t value) {
    PwmOut p((PinName)pin);
    p.write(value / 100.0f);
}

inline
static int digitalReadMbed(uint8_t pin) {
    DigitalIn p((PinName)pin);
    return int(p);
}

inline
static int digitalOutRead(uint8_t pin) {
    DigitalOut p((PinName)pin);
    return int(p);
}

inline
static void digitalWrite(uint8_t pin, int value) {
    DigitalOut p((PinName)pin);
    p = value;
}

template <class Proto, class Flash>
void ERaApi<Proto, Flash>::handleReadPin(cJSON* root) {
	if (!cJSON_IsArray(root)) {
		return;
	}

	PinConfig_t pin {};
	cJSON* current = nullptr;

	for (current = root->child; current != nullptr; current = current->next) {
		if (!cJSON_IsObject(current)) {
			continue;
		}
		pin = PinConfig_t();
		cJSON* item = cJSON_GetObjectItem(current, "config_id");
		if (cJSON_IsNumber(item)) {
			pin.configId = item->valueint;
		}
		item = cJSON_GetObjectItem(current, "pin_number");
		if (cJSON_IsNumber(item)) {
			pin.pin = item->valueint;
		}
        else if (cJSON_IsString(item)) {
            pin.pin = ERA_DECODE_PIN_NAME(item->valuestring);
        }
		pin.pinMode = this->getPinMode(current);
		if (pin.pinMode == VIRTUAL) {
			this->eraPinReport.setPinVirtual(pin.pin, pin.configId);
			continue;
		}
		if (!this->isReadPinMode(pin.pinMode)) {
			continue;
		}
		item = cJSON_GetObjectItem(current, "value_type");
		if (cJSON_IsString(item)) {
#if defined(FORCE_VIRTUAL_PIN)
			this->eraPinReport.setPinVirtual(pin.pin, pin.configId);
#else
			if (ERaStrCmp(item->valuestring, "boolean")) {
				this->getPinConfig(current, pin);
				this->eraPinReport.setPinReport(pin.pin, pin.pinMode, digitalReadMbed, pin.report.interval,
												pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange, this->reportPinConfigCb,
												pin.configId);
			}
			else if (ERaStrCmp(item->valuestring, "integer")) {
				pin.report = PinConfig_t::__ReportConfig_t(1000, 1000, 60000, 10.0f);
				this->getPinConfig(current, pin);
				this->getScaleConfig(current, pin);
				this->eraPinReport.setPinReport(pin.pin, ANALOG, analogReadMbed, pin.report.interval,
												pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange, this->reportPinConfigCb,
												pin.configId).setScale(pin.scale.min, pin.scale.max, pin.scale.rawMin, pin.scale.rawMax);
			}
#endif
		}
	}
}

template <class Proto, class Flash>
void ERaApi<Proto, Flash>::handleWritePin(cJSON* root) {
	if (!cJSON_IsArray(root)) {
		return;
	}

	PinConfig_t pin {};
	cJSON* current = nullptr;

	for (current = root->child; current != nullptr; current = current->next) {
		if (!cJSON_IsObject(current)) {
			continue;
		}
		pin = PinConfig_t();
		cJSON* item = cJSON_GetObjectItem(current, "config_id");
		if (cJSON_IsNumber(item)) {
			pin.configId = item->valueint;
		}
		item = cJSON_GetObjectItem(current, "pin_number");
		if (cJSON_IsNumber(item)) {
			pin.pin = item->valueint;
		}
        else if (cJSON_IsString(item)) {
            pin.pin = ERA_DECODE_PIN_NAME(item->valuestring);
        }
		pin.pinMode = this->getPinMode(current);
		if (pin.pinMode == VIRTUAL) {
			this->eraPinReport.setPinVirtual(pin.pin, pin.configId);
			continue;
		}
		if (this->isReadPinMode(pin.pinMode)) {
			continue;
		}
		item = cJSON_GetObjectItem(current, "value_type");
		if (cJSON_IsString(item)) {
#if defined(FORCE_VIRTUAL_PIN)
			this->eraPinReport.setPinVirtual(pin.pin, pin.configId);
#else
			if (ERaStrCmp(item->valuestring, "boolean")) {
				this->getPinConfig(current, pin);
				this->eraPinReport.setPinReport(pin.pin, pin.pinMode, digitalOutRead, pin.report.interval,
												pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange, this->reportPinConfigCb,
												pin.configId);
			}
			else if (ERaStrCmp(item->valuestring, "integer")) {
				this->getPinConfig(current, pin);
				this->getScaleConfig(current, pin);
                if (pin.pinMode == PWM) {
                    PwmOut p((PinName)pin.pin);
                    p.period(1.0f / pin.pwm.frequency);
                    p.write(0);
                }
                this->eraPinReport.setPinReport(pin.pin, PWM, nullptr, pin.report.interval,
                                                pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange, this->reportPinConfigCb,
                                                pin.configId).setScale(pin.scale.min, pin.scale.max, pin.scale.rawMin, pin.scale.rawMax);
			}
#endif
		}
	}
}

template <class Proto, class Flash>
void ERaApi<Proto, Flash>::processArduinoPinRequest(const std::vector<std::string>& arrayTopic, const std::string& payload) {
	if (arrayTopic.size() != 3) {
		return;
	}
	const std::string& str = arrayTopic.at(2);
	if (str.empty()) {
		return;
	}
	cJSON* root = cJSON_Parse(payload.c_str());
	if (!cJSON_IsObject(root)) {
		cJSON_Delete(root);
		root = nullptr;
		return;
	}
	uint8_t pin = ERA_DECODE_PIN_NAME(str.c_str());
	cJSON* item = cJSON_GetObjectItem(root, "value");
	if (cJSON_IsNumber(item)) {
		ERaParam raw;
		ERaParam param(item->valuedouble);
		float value = item->valuedouble;
		int pMode = this->eraPinReport.findPinMode(pin);
		const ERaReport::iterator* rp = this->eraPinReport.getReport(pin);
		if (rp != nullptr) {
			const ERaReport::ScaleData_t* scale = rp->getScale();
			if ((scale != nullptr) && scale->enable) {
				value = ERaMapNumberRange((float)item->valuedouble, scale->min, scale->max, scale->rawMin, scale->rawMax);
			}
		}
		switch (pMode) {
			case PWM:
			case ANALOG:
                ::pwmWrite(pin, value);
				if (rp != nullptr) {
					rp->updateReport(value);
				}
				break;
			case VIRTUAL:
			case ERA_VIRTUAL:
				this->callERaWriteHandler(pin, param);
				break;
			default:
				if (value == TOGGLE) {
					::digitalWrite(pin, ((digitalReadMbed(pin) == LOW) ? HIGH : LOW));
				}
				else {
					::digitalWrite(pin, value ? HIGH : LOW);
				}
				break;
		}
		if (pMode != ERA_VIRTUAL) {
			raw = value;
			this->callERaPinWriteHandler(pin, param, raw);
		}
	}
	else if (cJSON_IsString(item)) {
		ERaParam param;
		param.add_static(item->valuestring);
		this->callERaWriteHandler(pin, param);
	}

	cJSON_Delete(root);
	root = nullptr;
	item = nullptr;
}

template <class Proto, class Flash>
void ERaApi<Proto, Flash>::handlePinRequest(const std::vector<std::string>& arrayTopic, const std::string& payload) {
	cJSON* root = cJSON_Parse(payload.c_str());
	if (!cJSON_IsObject(root)) {
		cJSON_Delete(root);
		root = nullptr;
		return;
	}

	ERaParam param;
	PinConfig_t pin {};
	cJSON* current = nullptr;

	for (current = root->child; current != nullptr && current->string != nullptr; current = current->next) {
		if (this->getGPIOPin(current, "virtual_pin", pin.pin)) {
			if (cJSON_IsNumber(current)) {
				param = current->valuedouble;
			}
			else if (cJSON_IsString(current)) {
				param.add_static(current->valuestring);
			}
			this->callERaWriteHandler(pin.pin, param);
			continue;
		}
        if (getGPIOPin(current, "pin_mode", pin.pin)) {
            if (!cJSON_IsString(current)) {
                continue;
			}

			this->getReportConfig(root, pin);

            if (ERaStrCmp(current->valuestring, "output")) {
				DigitalOut p((PinName)pin.pin);
            }
			else if (ERaStrCmp(current->valuestring, "open_drain")) {
				DigitalOut p((PinName)pin.pin);
			}
            else if (ERaStrCmp(current->valuestring, "pwm")) {
                PwmOut p((PinName)pin.pin);
                p.period(1.0f / pin.pwm.frequency);
                p.write(0);
                this->eraPinReport.setPinReport(pin.pin, PWM, nullptr, pin.report.interval,
                                                pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange, this->reportPinConfigCb,
                                                pin.configId);
            }
            else if (ERaStrCmp(current->valuestring, "input")) {
				this->eraPinReport.setPinReport(pin.pin, INPUT, digitalReadMbed, pin.report.interval,
												pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange, this->reportPinCb);
            }
            else if (ERaStrCmp(current->valuestring, "pullup")) {
				this->eraPinReport.setPinReport(pin.pin, INPUT_PULLUP, digitalReadMbed, pin.report.interval,
												pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange, this->reportPinCb);
            }
            else if (ERaStrCmp(current->valuestring, "pulldown")) {
				this->eraPinReport.setPinReport(pin.pin, INPUT_PULLDOWN, digitalReadMbed, pin.report.interval,
												pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange, this->reportPinCb);
            }
			else if (ERaStrCmp(current->valuestring, "analog")) {
				this->eraPinReport.setPinReport(pin.pin, ANALOG, analogReadMbed, pin.report.interval,
												pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange, this->reportPinConfigCb,
												pin.configId);
            }
			else if (ERaStrCmp(current->valuestring, "remove")) {
				this->eraPinReport.deleteWithPin(pin.pin);
			}
            continue;
        }
        if (getGPIOPin(current, "digital_pin", pin.pin)) {
			ERaParam param(current->valueint);
			if (current->valueint == TOGGLE) {
				::digitalWrite(pin.pin, ((digitalReadMbed(pin.pin) == LOW) ? HIGH : LOW));
			}
			else {
            	::digitalWrite(pin.pin, current->valueint ? HIGH : LOW);
			}
            this->digitalWrite(pin.pin, digitalReadMbed(pin.pin));
			this->callERaPinWriteHandler(pin.pin, param, param);
            continue;
        }
		if (getGPIOPin(current, "pwm_pin", pin.pin)) {
            ::pwmWrite(pin.pin, current->valueint);
			continue;
		}
	}

	cJSON_Delete(root);
	root = nullptr;
}

#endif /* INC_ERA_API_MBED_HPP_ */
