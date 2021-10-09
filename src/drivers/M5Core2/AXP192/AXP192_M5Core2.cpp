#include "AXP192_M5Core2.h"

AXP192_M5Core2::AXP192_M5Core2()
{
}

void AXP192_M5Core2::begin(void)
{

    Wire1.begin(21, 22);
    Wire1.setClock(400000);

    log_w("[ AXP ]");

    //AXP192 30H
    Write1Byte(0x30, (Read8bit(0x30) & 0x04) | 0x02);
    log_d("  - VBUS limit off");

    //AXP192 GPIO1:OD OUTPUT
    Write1Byte(0x92, Read8bit(0x92) & 0xf8);
    log_d("  - GPIO1 init");

    //AXP192 GPIO2:OD OUTPUT
    Write1Byte(0x93, Read8bit(0x93) & 0xf8);
    log_d("  - GPIO2 init");

    //AXP192 RTC CHG
    Write1Byte(0x35, (Read8bit(0x35) & 0x1c) | 0xa3);
    log_w("  - RTC battery charging enabled");

    SetESPVoltage(3350);
    log_w("  - ESP32 power voltage was set to 3.35v");

    SetLcdVoltage(2800);
    log_w("  - TFT backlight voltage was set to 2.80v");

    SetLDOVoltage(2, 3300); //Periph power voltage preset (LCD_logic, SD card)
    log_w("  - TFT logic and SDCard voltage preset to 3.3v");

    SetLDOVoltage(3, 2000); //Vibrator power voltage preset
    log_w("  - Vibrator voltage preset to 2v");

    SetLDOEnable(2, true);

    SetCHGCurrent(kCHG_100mA);
    log_w("  - M5Go CHG Base current set to 100mA");

    //pinMode(39, INPUT_PULLUP);

    //AXP192 GPIO4
    Write1Byte(0x95, (Read8bit(0x95) & 0x72) | 0x84);
    log_d("  - GPIO4 init");

    Write1Byte(0x36, 0x4C);

    Write1Byte(0x82,0xff);

    SetLCDRSet(0);
    delay(100);
    SetLCDRSet(1);
    delay(100);
    // I2C_WriteByteDataAt(0X15,0XFE,0XFF);

    //  bus power mode_output
    SetBusPowerMode(0);
}

void AXP192_M5Core2::Write1Byte(uint8_t Addr, uint8_t Data)
{
    Wire1.beginTransmission(0x34);
    Wire1.write(Addr);
    Wire1.write(Data);
    Wire1.endTransmission();
}

uint8_t AXP192_M5Core2::Read8bit(uint8_t Addr)
{
    Wire1.beginTransmission(0x34);
    Wire1.write(Addr);
    Wire1.endTransmission();
    Wire1.requestFrom(0x34, 1);
    return Wire1.read();
}

uint16_t AXP192_M5Core2::Read12Bit(uint8_t Addr)
{
    uint16_t Data = 0;
    uint8_t buf[2];
    ReadBuff(Addr, 2, buf);
    Data = ((buf[0] << 4) + buf[1]); //
    return Data;
}

uint16_t AXP192_M5Core2::Read13Bit(uint8_t Addr)
{
    uint16_t Data = 0;
    uint8_t buf[2];
    ReadBuff(Addr, 2, buf);
    Data = ((buf[0] << 5) + buf[1]); //
    return Data;
}

uint16_t AXP192_M5Core2::Read16bit(uint8_t Addr)
{
    uint16_t ReData = 0;
    Wire1.beginTransmission(0x34);
    Wire1.write(Addr);
    Wire1.endTransmission();
    Wire1.requestFrom(0x34, 2);
    for (int i = 0; i < 2; i++)
    {
        ReData <<= 8;
        ReData |= Wire1.read();
    }
    return ReData;
}

uint32_t AXP192_M5Core2::Read24bit(uint8_t Addr)
{
    uint32_t ReData = 0;
    Wire1.beginTransmission(0x34);
    Wire1.write(Addr);
    Wire1.endTransmission();
    Wire1.requestFrom(0x34, 3);
    for (int i = 0; i < 3; i++)
    {
        ReData <<= 8;
        ReData |= Wire1.read();
    }
    return ReData;
}

uint32_t AXP192_M5Core2::Read32bit(uint8_t Addr)
{
    uint32_t ReData = 0;
    Wire1.beginTransmission(0x34);
    Wire1.write(Addr);
    Wire1.endTransmission();
    Wire1.requestFrom(0x34, 2);
    for (int i = 0; i < 4; i++)
    {
        ReData <<= 8;
        ReData |= Wire1.read();
    }
    return ReData;
}

void AXP192_M5Core2::ReadBuff(uint8_t Addr, uint8_t Size, uint8_t *Buff)
{
    Wire1.beginTransmission(0x34);
    Wire1.write(Addr);
    Wire1.endTransmission();
    Wire1.requestFrom(0x34, (int)Size);
    for (int i = 0; i < Size; i++)
    {
        *(Buff + i) = Wire1.read();
    }
}

void AXP192_M5Core2::ScreenBreath(uint8_t brightness)
{
    if (brightness > 12)
    {
        brightness = 12;
    }
    uint8_t buf = Read8bit(0x28);
    Write1Byte(0x28, ((buf & 0x0f) | (brightness << 4)));
}

bool AXP192_M5Core2::GetBatState()
{
    if (Read8bit(0x01) | 0x20)
        return true;
    else
        return false;
}
//---------coulombcounter_from_here---------
//enable: void EnableCoulombcounter(void);
//disable: void DisableCOulombcounter(void);
//stop: void StopCoulombcounter(void);
//clear: void ClearCoulombcounter(void);
//get charge data: uint32_t GetCoulombchargeData(void);
//get discharge data: uint32_t GetCoulombdischargeData(void);
//get coulomb val affter calculation: float GetCoulombData(void);
//------------------------------------------
void AXP192_M5Core2::EnableCoulombcounter(void)
{
    Write1Byte(0xB8, 0x80);
}

void AXP192_M5Core2::DisableCoulombcounter(void)
{
    Write1Byte(0xB8, 0x00);
}

void AXP192_M5Core2::StopCoulombcounter(void)
{
    Write1Byte(0xB8, 0xC0);
}

void AXP192_M5Core2::ClearCoulombcounter(void)
{
    Write1Byte(0xB8, 0xA0);
}

uint32_t AXP192_M5Core2::GetCoulombchargeData(void)
{
    return Read32bit(0xB0);
}

uint32_t AXP192_M5Core2::GetCoulombdischargeData(void)
{
    return Read32bit(0xB4);
}

float AXP192_M5Core2::GetCoulombData(void)
{

    uint32_t coin = 0;
    uint32_t coout = 0;

    coin = GetCoulombchargeData();
    coout = GetCoulombdischargeData();

    //c = 65536 * current_LSB * (coin - coout) / 3600 / ADC rate
    //Adc rate can be read from 84H ,change this variable if you change the ADC reate
    float ccc = 65536 * 0.5 * (coin - coout) / 3600.0 / 25.0;
    return ccc;
}

void AXP192_M5Core2::SetSleep(void)
{
    uint8_t buf = Read8bit(0x31);
    buf = (1 << 3) | buf;
    Write1Byte(0x31, buf);
    Write1Byte(0x90, 0x00);
    Write1Byte(0x12, 0x09);
    Write1Byte(0x12, 0x00);
}

uint8_t AXP192_M5Core2::GetWarningLeve(void)
{
    Wire1.beginTransmission(0x34);
    Wire1.write(0x47);
    Wire1.endTransmission();
    Wire1.requestFrom(0x34, 1);
    uint8_t buf = Wire1.read();
    return (buf & 0x01);
}

// -- sleep
void AXP192_M5Core2::DeepSleep(uint64_t time_in_us)
{
    SetSleep();

    if (time_in_us > 0)
    {
        esp_sleep_enable_timer_wakeup(time_in_us);
    }
    else
    {
        esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TIMER);
    }
    (time_in_us == 0) ? esp_deep_sleep_start() : esp_deep_sleep(time_in_us);
}

void AXP192_M5Core2::LightSleep(uint64_t time_in_us)
{
    SetSleep();

    if (time_in_us > 0)
    {
        esp_sleep_enable_timer_wakeup(time_in_us);
    }
    else
    {
        esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TIMER);
    }
    esp_light_sleep_start();
}

uint8_t AXP192_M5Core2::GetWarningLevel(void)
{
    return Read8bit(0x47) & 0x01;
}

float AXP192_M5Core2::GetBatVoltage()
{
    float ADCLSB = 1.1 / 1000.0;
    uint16_t ReData = Read12Bit(0x78);
    return ReData * ADCLSB;
}

float AXP192_M5Core2::GetBatCurrent()
{
    float ADCLSB = 0.5;
    uint16_t CurrentIn = Read13Bit(0x7A);
    uint16_t CurrentOut = Read13Bit(0x7C);
    return (CurrentIn - CurrentOut) * ADCLSB;
}

float AXP192_M5Core2::GetVinVoltage()
{
    float ADCLSB = 1.7 / 1000.0;
    uint16_t ReData = Read12Bit(0x56);
    return ReData * ADCLSB;
}

float AXP192_M5Core2::GetVinCurrent()
{
    float ADCLSB = 0.625;
    uint16_t ReData = Read12Bit(0x58);
    return ReData * ADCLSB;
}

float AXP192_M5Core2::GetVBusVoltage()
{
    float ADCLSB = 1.7 / 1000.0;
    uint16_t ReData = Read12Bit(0x5A);
    return ReData * ADCLSB;
}

float AXP192_M5Core2::GetVBusCurrent()
{
    float ADCLSB = 0.375;
    uint16_t ReData = Read12Bit(0x5C);
    return ReData * ADCLSB;
}

float AXP192_M5Core2::GetTempInAXP192()
{
    float ADCLSB = 0.1;
    const float OFFSET_DEG_C = -144.7;
    uint16_t ReData = Read12Bit(0x5E);
    return OFFSET_DEG_C + ReData * ADCLSB;
}

float AXP192_M5Core2::GetBatPower()
{
    float VoltageLSB = 1.1;
    float CurrentLCS = 0.5;
    uint32_t ReData = Read24bit(0x70);
    return VoltageLSB * CurrentLCS * ReData / 1000.0;
}

float AXP192_M5Core2::GetBatChargeCurrent()
{
    float ADCLSB = 0.5;
    uint16_t ReData = Read12Bit(0x7A);
    return ReData * ADCLSB;
}
float AXP192_M5Core2::GetAPSVoltage()
{
    float ADCLSB = 1.4 / 1000.0;
    uint16_t ReData = Read12Bit(0x7E);
    return ReData * ADCLSB;
}

float AXP192_M5Core2::GetBatCoulombInput()
{
    uint32_t ReData = Read32bit(0xB0);
    return ReData * 65536 * 0.5 / 3600 / 25.0;
}

float AXP192_M5Core2::GetBatCoulombOut()
{
    uint32_t ReData = Read32bit(0xB4);
    return ReData * 65536 * 0.5 / 3600 / 25.0;
}

void AXP192_M5Core2::SetCoulombClear()
{
    Write1Byte(0xB8, 0x20);
}

void AXP192_M5Core2::SetLDO2(bool State)
{
    uint8_t buf = Read8bit(0x12);
    if (State == true)
        buf = (1 << 2) | buf;
    else
        buf = ~(1 << 2) & buf;
    Write1Byte(0x12, buf);
}

uint8_t AXP192_M5Core2::AXPInState()
{
    return Read8bit(0x00);
}
bool AXP192_M5Core2::isACIN()
{
    return ( Read8bit(0x00) & 0x80 ) ? true : false;
}
bool AXP192_M5Core2::isCharging()
{
    return ( Read8bit(0x00) & 0x04 ) ? true : false;

}
bool AXP192_M5Core2::isVBUS()
{
    return ( Read8bit(0x00) & 0x20 ) ? true : false;
}

void AXP192_M5Core2::SetLDOVoltage(uint8_t number, uint16_t voltage)
{
    voltage = (voltage > 3300) ? 15 : (voltage / 100) - 18;
    switch (number)
    {
    //uint8_t reg, data;
    case 2:
        Write1Byte(AXP_ADDR, (Read8bit(0x28) & 0x0F) | (voltage << 4));
        break;
    case 3:
        Write1Byte(AXP_ADDR, (Read8bit(0x28) & 0xF0) | voltage);
        break;
    }
}

void AXP192_M5Core2::SetDCVoltage(uint8_t number, uint16_t voltage)
{
    uint8_t addr;
    if (number > 2)
        return;
    voltage = (voltage < 700) ? 0 : (voltage - 700) / 25;
    switch (number)
    {
    case 0:
        addr = 0x26;
        break;
    case 1:
        addr = 0x25;
        break;
    case 2:
        addr = 0x27;
        break;
    }
    Write1Byte(addr, (Read8bit(addr) & 0x80) | (voltage & 0x7F));
}

void AXP192_M5Core2::SetESPVoltage(uint16_t voltage)
{
    if (voltage >= 3000 && voltage <= 3400)
    {
        SetDCVoltage(0, voltage);
    }
}
void AXP192_M5Core2::SetLcdVoltage(uint16_t voltage)
{
    if (voltage >= 2500 && voltage <= 3300)
    {
        SetDCVoltage(2, voltage);
    }
}

void AXP192_M5Core2::SetLDOEnable(uint8_t number, bool state)
{
    uint8_t mark = 0x01;
    if ((number < 2) || (number > 3))
        return;

    mark <<= number;
    if (state)
    {
        Write1Byte(0x12, (Read8bit(0x12) | mark));
    }
    else
    {
        Write1Byte(0x12, (Read8bit(0x12) & (~mark)));
    }
}

void AXP192_M5Core2::SetLCDRSet(bool state)
{
    uint8_t reg_addr = 0x96;
    uint8_t gpio_bit = 0x02;
    uint8_t data;
    data = Read8bit(reg_addr);

    if (state)
    {
        data |= gpio_bit;
    }
    else
    {
        data &= ~gpio_bit;
    }

    Write1Byte(reg_addr, data);
}

void AXP192_M5Core2::SetBusPowerMode(uint8_t state)
{
    uint8_t data;
    if (state == 0)
    {
        data = Read8bit(0x91);
        Write1Byte(0x91, (data & 0x0F) | 0xF0);
        //set GPIO0 to LDO OUTPUT , pullup N_VBUSEN to disable supply from BUS_5V
        data = Read8bit(0x90);
        Write1Byte(0x90, (data & 0xF8) | 0x02);
        //set EXTEN to enable 5v boost
        data = Read8bit(0x10);
        Write1Byte(0x10, data | 0x04);
    }
    else
    {
        // Set EXTEN to disable 5v boost
        data = Read8bit(0x10);
        Write1Byte(0x10, data & ~0x04);
        // Set GPIO0 to float, using enternal pulldown resistor to enable VBUS supply from BUS_5V
        data = Read8bit(0x90);
        Write1Byte(0x90, (data & 0xF8) | 0x07);
    }
}

void AXP192_M5Core2::SetLed(uint8_t state)
{
    uint8_t reg_addr=0x94;
    uint8_t data;
    data=Read8bit(reg_addr);

    if(state)
    {
      data=data&0xFD;
    }
    else
    {
      data|=0x02;
    }

    Write1Byte(reg_addr,data);
}

//set led state(GPIO high active,set 1 to enable amplifier)
void AXP192_M5Core2::SetSpkEnable(uint8_t state)
{
    uint8_t reg_addr=0x94;
    uint8_t gpio_bit=0x04;
    uint8_t data;
    data=Read8bit(reg_addr);

    if(state)
    {
      data|=gpio_bit;
    }
    else
    {
      data&=~gpio_bit;
    }

    Write1Byte(reg_addr,data);
}

void AXP192_M5Core2::SetCHGCurrent(uint8_t state)
{
    uint8_t data = Read8bit(0x33);
    data &= 0xf0;
    data = data | ( state & 0x0f );
    Write1Byte(0x33,data);
}

// Cut all power, except for LDO1 (RTC)
void AXP192_M5Core2::PowerOff()
{
    Write1Byte(0x32, Read8bit(0x32) | 0x80);     // MSB for Power Off
}
