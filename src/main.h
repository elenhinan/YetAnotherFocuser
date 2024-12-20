#pragma once
#include <Arduino.h>
#include "config.h"
#include "pins.h"

#include <ESP_WiFiManager.h>
#include <ESP_DoubleResetDetector.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>
#include <SPI.h>
#include <TMCStepper.h>
#include <FastAccelStepper.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include "focuser.h"
#include "AlpacaServer.h"

// 
bool encA, encB;
volatile int32_t enc_counter = 0;
DeviceAddress* temp_address;
uint8_t n_sensors;
char buffer[128];

// module setup
void setup_wifi();
void setup_encoder();
void setup_sensors();
void setup_ota();

// module housekeeping
void update_encoder();
void update_sensors();
void update_focus();

// interrupt routines
void ICACHE_RAM_ATTR encoderA_isr();
void ICACHE_RAM_ATTR encoderB_isr();