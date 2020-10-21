#include "Vcc.h"

Vcc::Vcc()
{
    _refCalibration = VCC_DEFAULT_REFERENCE_CALIBRATION;
}

float Vcc::getValue()
{
    return getValue_mV() / 1000.0;
}

uint16_t Vcc::getValue_mV()
{
    return _refCalibration / getInternal();
}

float Vcc::getPinVoltage(uint8_t pin)
{
    return analogRead(pin) / 1024 * getValue_mV() / 1000.0;
}

uint16_t Vcc::getPinVoltage_mV(uint8_t pin)
{
    return analogRead(pin) / 1024 * getValue_mV() / 1000.0;
}

float Vcc::getPinVoltage(uint8_t pin, float r1, float r2)
{
    return getPinVoltage(pin) / (r2 / (r1 + r2));
}

uint16_t Vcc::getPinVoltage_mV(uint8_t pin, float r1, float r2)
{
    return getPinVoltage_mV(pin) / (r2 / (r1 + r2));
}

void Vcc::applyCalibration(int32_t value)
{
    _refCalibration = value;
}

uint32_t Vcc::calculateCalibrationConstant(uint16_t milliVolts)
{
    return (uint32_t)milliVolts * getInternal();
}

#ifdef VCC_EEPROM
bool Vcc::loadCalibrationFromEEPROM(int16_t address)
{
    uint8_t signature = EEPROM.read(address);

    if (signature == VCC_EEPROM_SIGNATURE)
    {
        EEPROM.get(address + 1, _refCalibration);
        return true;
    }
    return false;
}
void Vcc::storeCalibrationInEEPROM(int16_t address, int32_t value)
{
    int16_t addrValue = address + 1;
    int32_t storedValue = 0;

    EEPROM.get(addrValue, storedValue);
    if (storedValue != value)
    {
        EEPROM.put(addrValue, value);
    }
    EEPROM.update(address, VCC_EEPROM_SIGNATURE);
}
#endif

uint16_t Vcc::getInternal()
{
    if (ADMUX != ADMUX_VCCWRT1V1)
    {
        ADMUX = ADMUX_VCCWRT1V1;
        delayMicroseconds(500);
    }

    ADCSRA |= _BV(ADSC);
    while (bit_is_set(ADCSRA, ADSC))
    {
    };

    uint8_t low = ADCL;
    uint8_t high = ADCH;

    return (high << 8) | low;
}