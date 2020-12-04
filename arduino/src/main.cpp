#include <TheThingsNetwork.h>
#include <TheThingsMessage.h>
#include <CayenneLPP.h>
#include <SoftwareSerial.h>

// Set your AppEUI and AppKey
const char *appEui = "70B3D57ED0038A78";
const char *appKey = "34F400F96D8DE414E8099449B661EC25";

SoftwareSerial Serial1(10, 11); // RX, TX (of arduino)

#define loraSerial Serial1
#define debugSerial Serial

devicedata_t data = api_DeviceData_init_default;
void message(const uint8_t *payload, size_t length, port_t port);

#define GazPin A0

// Replace REPLACE_ME with TTN_FP_EU868 or TTN_FP_US915
#define freqPlan TTN_FP_EU868

TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan);

void setup()
{
  loraSerial.begin(57600);
  debugSerial.begin(9600);

  // Wait a maximum of 10s for Serial Monitor
  while (!debugSerial && millis() < 10000);

  pinMode(GazPin, INPUT);

  debugSerial.println("-- STATUS");
  ttn.showStatus();

  debugSerial.println("-- JOIN");
  ttn.join(appEui, appKey);

  ttn.onMessage(message);

  ttn.showStatus();
  debugSerial.println("Setup for The Things Network complete");
}

void loop()
{

  uint8_t gaz = analogRead(GazPin);

  /* Split word (16 bits) into 2 bytes of 8
  byte payload[2];
  payload[0] = highByte(light);
  payload[1] = lowByte(light);*/

  debugSerial.print("Transmitting gaz level: ");
  debugSerial.println(gaz);

  ttn.sendBytes(&gaz, sizeof(gaz));

  delay(20000);
}

void message(const uint8_t *payload, size_t length, port_t port)
{
  //standard message always received on port 100 or more
  if (port >= 100)
  {
    appdata_t appData = api_AppData_init_default;
    TheThingsMessage::decodeAppData(&appData, payload, length);
    debugSerial.print("Received: ");
    debugSerial.println(*payload);
  }
}
