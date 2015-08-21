#include <Arduino.h>
#include <ArduinoPebbleSerial.h>

#define BUTTON_PIN        7
#define DATA_PIN          8
#define DEBOUNCE_INTERVAL 500

static uint8_t s_data_buffer[128];
static long s_last_time = 0;

static const uint16_t s_service_ids[] = {0x0};
static const uint16_t s_attr_ids[] = {0x0};

void setup() {
  pinMode(13, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  
  // Setup the Pebble smartstrap connection using one wire software serial
  ArduinoPebbleSerial::begin_software(DATA_PIN, s_data_buffer, sizeof(s_data_buffer),
                                      Baud57600, s_service_ids, 1);
}

void loop() {
  size_t length;
  RequestType type;
  uint16_t service_id;
  uint16_t attribute_id;
  
  // Check for frames, though none are expected for this app
  ArduinoPebbleSerial::feed(&service_id, &attribute_id, &length, &type);

  // If the button is pressed...
  if(digitalRead(BUTTON_PIN) == HIGH) {
    long now = millis();
    if(now - s_last_time > DEBOUNCE_INTERVAL) {
      s_last_time = now;
     
      // Notify the watch!
      ArduinoPebbleSerial::notify(s_service_ids[0], s_attr_ids[0]);
    }
  }

  // Show connected status on the LED
  digitalWrite(13, ArduinoPebbleSerial::is_connected() ? HIGH : LOW);
}
