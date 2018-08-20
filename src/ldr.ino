#include <ESP8266WiFi.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include 'config.h'

WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, mqtt_server, mqtt_port, mqtt_username, mqtt_passwd);

Adafruit_MQTT_Publish ldr = Adafruit_MQTT_Publish(&mqtt, mqtt_username "/iot/ldr");

void MQTT_connect();

void setup() {
  Serial.begin(115200);
  pinMode(BUILTIN_LED, OUTPUT);
  pinMode(4, INPUT);
  Serial.println("LDR MQTT Client");
  
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());
  blink(5);
}

int ldr_val=0;
int past_ldr=0;

void loop() {
  ldr_val = digitalRead(4);
  
  if (ldr_val != past_ldr) {
    MQTT_connect();
    
    Serial.print(F("\nSending LDR val "));
    Serial.print(ldr_val);
    Serial.print("...");
    if (! ldr.publish(ldr_val)) {
      Serial.println(F("Failed"));
    } else {
      Serial.println(F("OK!"));
      past_ldr = ldr_val;
    }
  }
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}

void blink(int times) {
  uint8_t i = 0;

  for(i=0; i < times; i++) {
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
  }
}

