#include <DallasTemperature.h>
#include <Ethernet.h>
#include <OneWire.h>
#include <PString.h>
#include <PubSubClient.h>
#include <SPI.h>

#define ONE_WIRE_DIGITAL_PORT 2
#define MAX_SENSORS 6

#define MQTT_CLID TempSensor
#define MQTT_USER tempArduino
#define MQTT_PASS tempArduino

//Network settings
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte local_ip[] = { 192, 168, 1, 82 };
byte remote_ip[] = { 192, 168, 1, 50 };

OneWire oneWire(ONE_WIRE_DIGITAL_PORT);
DallasTemperature sensors(&oneWire);

void callback(char* topic, byte* payload, unsigned int length);

EthernetClient ethClient;
PubSubClient client(remote_ip, 1883, callback, ethClient);

int device_count = 0;

DeviceAddress devices[MAX_SENSORS];

void setup() {
    Serial.begin(9600);
    Serial.println("Initializing OneWire MQTT Remote");

    Serial.println("Initializing internet...");
    Ethernet.begin(mac, local_ip);
    Serial.println("Done!");

    Serial.println("Connecting to MQTT...");
    if (client.connect("arduinoClient", "tempArduino", "tempArduino")) {
        Serial.println("Success!");
    } else {
        Serial.println("Failure!");
    }

    Serial.println("Searching for devices...");
    sensors.begin();
    Serial.println("Done!");

    device_count = sensors.getDeviceCount();

    Serial.print("Found devices: ");
    Serial.print(device_count);
    Serial.println();
    if (device_count > MAX_SENSORS) {
        Serial.print("Too many sensors! Only monitoring first ");
        Serial.print(MAX_SENSORS);
        Serial.println("!");
        device_count = MAX_SENSORS;
    }

    for(int i=0; i < device_count; i++) {
        sensors.getAddress(devices[i], i);
    }
}

void loop() {
    sensors.requestTemperatures();

    for(int i=0; i < device_count; i++) {
        float temp = sensors.getTempF(devices[i]);
        Serial.print("Sensor ");
        for (uint8_t offset = 0; offset < 8; offset++)
        {
            // zero pad the address if necessary
            if (devices[i][offset] < 16) Serial.print("0");
            Serial.print(devices[i][offset], HEX);
            if (offset < 7) Serial.print(":");
        }
        Serial.print(" : ");
        Serial.print(temp);
        Serial.println("F");
        char buf[256];
        PString str(buf, sizeof(buf));
        str.print("sensors/");
        for (uint8_t offset = 0; offset < 8; offset++)
        {
            // zero pad the address if necessary
            if (devices[i][offset] < 16) str.print("0");
            str.print(devices[i][offset], HEX);
            if (offset < 7) str.print(":");
        }
        str.print("/temp");
        char tempBuf[30];
        PString tempStr(tempBuf, sizeof(tempBuf));
        tempStr.print(temp, DEC);
        client.publish(buf, tempBuf);
    }

    Serial.println();
}

void callback(char* topic, byte* payload, unsigned int length) {
      // handle message arrived
}
