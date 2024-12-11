#pragma once

// Stepper hw setup
#define PIN_TMC1_EN     16
#define PIN_TMC1_STEP   13
#define PIN_TMC1_DIR    14
#define PIN_TMC1_DIAG   35
#define PIN_TMC2_EN     26
#define PIN_TMC2_STEP   27
#define PIN_TMC2_DIR    25
#define PIN_TMC2_DIAG   34
#define PIN_TMC_RXD     19 // should use uart2 pin 16 instead
#define PIN_TMC_TXD     21 // should use uart2 pin 17 instead
#define ADDR_TMC1       0b00
#define ADDR_TMC2       0b01
#define SERIAL_TMC      SERIAL

// Servos
#define PIN_SRV1     5 // pwm output on start, move to other pin
#define PIN_SRV2     18

// Rotary encoder
#define PIN_ENC_A    39
#define PIN_ENC_B    36

// Power down detector
#define PIN_PD       32

// I2C
#define PIN_SDA      23
#define PIN_SCL      22

// IO expander interrupt
#define PIN_INTR     15

// DS1820 Temperature sensors
#define PIN_ONEWIRE  4

// LEDS
#define PIN_LED      2
#define PIN_WIFI_LED LED_BUILTIN