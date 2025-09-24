#include "RMaker.h"
#include "WiFi.h"
#include "WiFiProv.h"
#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#include "MAX30105.h"
#include "heartRate.h"
#include <HardwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include <MPU6050.h>
#include <HUSKYLENS.h>
HardwareSerial GPS(2);
MPU6050 mpu;


HUSKYLENS huskylens;
void printResult(HUSKYLENSResult result);

// ---------- DFPlayer ----------
HardwareSerial DFSerial(1);  // UART1
DFRobotDFPlayerMini myDFPlayer;
#define DF_RX 26
#define DF_TX 25

String nmeaSentence = "";

MAX30105 particleSensor;
SSD1306AsciiWire oled;

#define buzzer 13
#define panic_button 15


const char *service_name = "Care Companion";
const char *pop = "companion";
static Device alert("Alert", "custom.device.med");
static Device heartRate("Heart", "custom.device.hrate");
static Device notify("Notify", "custom.device.notify");


String device_name = "", param_name = "", received_message = "";
bool incoming_message = 0, incoming_value = 0, fallDetected = 0;
bool fall_detect = 0, panic_mode = 0, alarm_done = 1;
int beatAvg = 60;
int SpO2 = 95;

int med_cat = 0;

uint32_t panic_timer = millis(), prep_time = millis(), fall_pause, last_fall, health_timer = millis();
int step = 1;


void setup() {
  Serial.begin(115200);
  GPS.begin(9600, SERIAL_8N1, 16, 17);  // RX=16, TX=17
  DFSerial.begin(9600, SERIAL_8N1, DF_RX, DF_TX);
  if (!myDFPlayer.begin(DFSerial)) Serial.println("DFPlayer init failed. Check wiring and SD card.");

  Wire.begin();
  Wire.setClock(400000L);
  mpu.initialize();
  if (!mpu.testConnection()) Serial.println("MPU6050 connection failed");
  particleSensor.begin(Wire, I2C_SPEED_FAST);
  particleSensor.setup(0x7F, 4, 3, 800, 215, 16384);
  particleSensor.enableDIETEMPRDY();
  particleSensor.setPulseAmplitudeRed(0x0A);

  oled.begin(&Adafruit128x64, 0x3C);
  oled.setFont(System5x7);
  oled.set2X();
  oled.clear();
  text("   Care   ", 4, 2);
  text("Companion", 10, 5);

  myDFPlayer.volume(40);
  myDFPlayer.play(4);
  pinMode(0, INPUT_PULLUP);
  pinMode(panic_button, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);

  Node my_node;
  my_node = RMaker.initNode("Care Companion");

  Param fall_det("Fall Detect", "custom.param.fall", value(bool(0)), PROP_FLAG_READ);
  Param panic("Panic Mode", "custom.param.panic", value(bool(0)), PROP_FLAG_READ);
  Param status("status", "esp.param.ota_status", value("Active"), PROP_FLAG_READ | PROP_FLAG_WRITE);
  Param number("Message to", "esp.param.ota_status", value("01759422284"), PROP_FLAG_READ);
  Param sms("SMS", "esp.param.ota_status", value("Type Anything Here!"), PROP_FLAG_READ | PROP_FLAG_WRITE);
  Param pulse_val("Pulse Value", "custom.param.pulseval", value(beatAvg), PROP_FLAG_READ);
  Param oxi_val("Oximeter Value", "custom.param.oxival", value(SpO2), PROP_FLAG_READ);

  alert.addPowerParam(0, "Reminder");
  alert.addPowerParam(0, "Morning");
  alert.addPowerParam(0, "Afternoon");
  alert.addPowerParam(0, "Night");
  alert.addParam(fall_det);
  alert.addParam(panic);
  notify.addParam(status);
  notify.addParam(number);
  notify.addParam(sms);
  heartRate.addParam(pulse_val);
  heartRate.addParam(oxi_val);

  alert.addCb(write_callback);
  notify.addCb(write_callback);


  my_node.addDevice(alert);
  my_node.addDevice(heartRate);
  my_node.addDevice(notify);

  RMaker.enableOTA(OTA_USING_PARAMS);
  RMaker.setTimeZone("Asia/Dhaka");
  RMaker.enableSchedule();
  RMaker.enableScenes();
  RMaker.start();

  WiFi.onEvent(sysProvEvent);
  WiFiProv.beginProvision(WIFI_PROV_SCHEME_BLE, WIFI_PROV_SCHEME_HANDLER_FREE_BTDM, WIFI_PROV_SECURITY_1, pop, service_name);


  if (!huskylens.begin(Wire)) Serial.println(F("HUSKYLENS not connected!"));


  long seed = analogRead(35);
  seed ^= (esp_random());
  randomSeed(seed);
}

void loop() {
  input_processing();
  device_reset();
  delay(100);

  if (!digitalRead(panic_button) && !panic_mode) {
    Serial.println("panic!");
    panic_mode = 1;
    alert.updateAndReportParam("Panic Mode", panic_mode);
    panic_timer = millis();
    call_phone();
  }

  if (GPS.available()) {
    char c = GPS.read();
    if (c == '\n') {
      parseNMEA(nmeaSentence);
      nmeaSentence = "";
    } else if (c != '\r') {
      nmeaSentence += c;
    }
  }

  if (millis() - panic_timer > 5000 && panic_mode) {
    panic_mode = 0;
    alert.updateAndReportParam("Panic Mode", panic_mode);
  }

  sms_prep();
  mpu_read();

  if (millis() - health_timer > 30000) {
    bool x = full_checkup();
    if (x) {
      heartRate.updateAndReportParam("Pulse Value", beatAvg);
      heartRate.updateAndReportParam("Oximeter Value", SpO2);
    }

    else {
      notify.updateAndReportParam("status", "Missed Reading");
      delay(5000);
      notify.updateAndReportParam("status", "Active");
    }

    oled.clear();
    text("   Care   ", 4, 2);
    text("Companion", 10, 5);
    health_timer = millis();
  }
}
