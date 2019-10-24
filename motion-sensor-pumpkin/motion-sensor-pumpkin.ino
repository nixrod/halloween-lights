#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define wifi_ssid "YOUR_WIFI_SSID"
#define wifi_password "YOUR_WIFI_PASSWORD"

#define mqtt_server "YOUR_MQTT_SERVER"

#define topic "movement"

WiFiClient espClient;
PubSubClient client(espClient);
int sensor = 13;

void setup() {
  Serial.begin(115200);
  pinMode(sensor, INPUT);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("Pir Sensor")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long state = digitalRead(sensor);
  if (state == HIGH) {
    Serial.println("Movement detected, sending message");
    client.publish(topic, "Movement", true);
    delay(3000);
  }
 
}
