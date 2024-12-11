#include "main.h"

#ifdef FOCUSER2_ENABLE
  #define N_FOCUSERS 2
#else
  #define N_FOCUSERS 1
#endif

OneWire oneWire(PIN_ONEWIRE);
DallasTemperature sensors(&oneWire);
Focuser focuser[N_FOCUSERS] = {
  Focuser(&TMC_SERIAL, PIN_TMC_RXD, PIN_TMC_TXD, PIN_TMC1_STEP, PIN_TMC1_DIR, PIN_TMC1_EN, PIN_TMC1_DIAG)
#ifdef FOCUSER2_ENABLE
  ,Focuser(&TMC_SERIAL, PIN_TMC_RXD, PIN_TMC_TXD, PIN_TMC2_STEP, PIN_TMC2_DIR, PIN_TMC2_EN, PIN_TMC2_DIAG)
#endif
};
WiFiServer tcpServer(TCP_PORT);
WiFiClient tcpClient;

AlpacaServer alpacaServer("AstroFocus");

void setup() {
  // setup serial
  Serial.begin(115200, SERIAL_8N1);

  setup_wifi();

  // setup ASCOM Alpaca server
  alpacaServer.begin(ALPACA_UDP_PORT, ALPACA_TCP_PORT);
  // add devices
  for(uint8_t i=0; i<N_FOCUSERS; i++) {
    focuser[i].begin();
    alpacaServer.addDevice(&focuser[i]);
  }
  // load settings
  alpacaServer.loadSettings();

  setup_encoder();
  setup_sensors();
}

void loop() {
  update_encoder();
  update_sensors();
  update_focus();
  ArduinoOTA.handle();
  delay(50); 
}

void setup_wifi()
{
  pinMode(PIN_WIFI_LED, OUTPUT);

  // setup wifi
  Serial.print(F("\n# Starting WiFi"));

  DoubleResetDetector drd = DoubleResetDetector(DRD_TIMEOUT, DRD_ADDRESS);
  ESP_WiFiManager ESP_wifiManager(HOSTNAME);
  ESP_wifiManager.setConnectTimeout(60);

  if (ESP_wifiManager.WiFi_SSID() == "" || drd.detectDoubleReset()) {
    Serial.println(F("# Starting Config Portal"));
    digitalWrite(PIN_WIFI_LED, HIGH);
    if (!ESP_wifiManager.startConfigPortal()) {
      Serial.println(F("# Not connected to WiFi"));
    } else {
      Serial.println(F("# connected"));
    }
  } else {
    WiFi.mode(WIFI_STA);
    WiFi.begin();
  } 
  WiFi.waitForConnectResult();
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println(F("# Failed to connect"));
  }
  else {
    Serial.print(F("# Local IP: "));
    Serial.println(WiFi.localIP());
    digitalWrite(PIN_WIFI_LED, HIGH);
    if(!MDNS.begin("HOSTNAME")) {
     Serial.println("# Error starting mDNS");
     return;
    }
  }
}

void setup_ota()
{
  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();
}

void setup_encoder()
{
  pinMode(PIN_ENC_A, INPUT_PULLUP);
  pinMode(PIN_ENC_B, INPUT_PULLUP);
  encA = digitalRead(PIN_ENC_A);
  encB = digitalRead(PIN_ENC_B);
  attachInterrupt(PIN_ENC_A, encoderA_isr, CHANGE);
  attachInterrupt(PIN_ENC_B, encoderB_isr, CHANGE);
}

void setup_sensors()
{
  sensors.begin();
  // find sensors
  n_sensors = sensors.getDS18Count();
  temp_address = new DeviceAddress[n_sensors]; // allocate array
  sprintf(buffer,"Found %d temperature sensor(s)", n_sensors);
  Serial.println(buffer);
  for(int i=0;i<n_sensors;i++) {
    sensors.getAddress(temp_address[i], i);
    Serial.println("test");
    for(int j=0;j<8;j++)
      Serial.print(temp_address[i][j],HEX);
    //sprintf(buffer, " [%d] 0x%016X",i,*((uint64_t*)temp_address[i]));
    //Serial.println(buffer);
  }
  
  if (n_sensors > 0) {
    Serial.println("setting up sensors");
    sensors.setResolution(12);
    sensors.setWaitForConversion(false);
    sensors.setCheckForConversion(true);
  }
}

void update_encoder()
{
  if(enc_counter) {
    uint8_t fIndex = 0;//digitalRead(PIN_ENC_SEL);
    focuser[fIndex].move(enc_counter);
    enc_counter = 0;
  }
}

void update_sensors()
{
  if(sensors.isConversionComplete()) {
    //Serial.println("reading sensors");
    float temperature;
    for(int i=0; i<n_sensors; i++) {
      temperature = sensors.getTempC(temp_address[i]);
      if(i<N_FOCUSERS)
        focuser[i].setTemperature(temperature);
    }
    sensors.requestTemperatures();
  } else {
    //Serial.println("Sensor not ready");
  }
}

void update_focus()
{
  for(int i=0;i<N_FOCUSERS;i++) {
    focuser[i].update();
  }
}

// Interrupts
void ICACHE_RAM_ATTR encoderA_isr() {
    encA = digitalRead(PIN_ENC_A);
    if (encA == encB) {
      enc_counter++;
    } else {
      enc_counter--;
    }
}

void ICACHE_RAM_ATTR encoderB_isr() {
    encB = digitalRead(PIN_ENC_B);
    if (encA == encB) {
      enc_counter--;
    } else {
      enc_counter++;
    }
}