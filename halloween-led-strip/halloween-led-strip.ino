#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <FastLED.h>

#define wifi_ssid "YOUR_WIFI_SSID"
#define wifi_password "YOUR_WIFI_PASSWORD"

#define mqtt_server "YOUR_MQTT_SERVER"

#define LED_PIN     D2
#define NUM_LEDS    300

#define BRIGHTNESS  200
#define FRAMES_PER_SECOND 60

CRGB leds[NUM_LEDS];
String theme = "fire";
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
CRGBPalette16 currentPalette;
// Array of temperature readings at each simulation cell
static byte heat[NUM_LEDS];

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness( BRIGHTNESS );
  
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
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

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
  }
  Serial.println();

  if (String(topic) == "horror") {
    Serial.println("Playing horror");
    theme = "horror";
    currentPalette = LavaColors_p;
  } else if (String(topic) == "fire") {
    Serial.println("Playing fire");
    theme = "fire";
    memset(heat,0,sizeof(heat));
  } else if (String(topic) == "pink") {
    Serial.println("Playing pink");
    theme = "pink";

    CRGB purple = CRGB(0x4B0082);
    CRGB medVio = CRGB(0xC71585);
    CRGB pink  = CRGB(0xFF1493);
    CRGB lightPink  = CRGB(0xFF69B4);
    
    currentPalette = CRGBPalette16(
                         lightPink,  lightPink,  pink,  pink,
                         medVio, medVio, purple,  purple,
                         lightPink,  lightPink,  pink,  pink,
                         medVio, medVio, purple,  purple );
  } else if (String(topic) == "off") {
    Serial.println("Turning off");
    theme = "off";
  }
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("LedStrip")) {
      Serial.println("connected");
      client.subscribe("fire");
      client.subscribe("horror");
      client.subscribe("pink");
      client.subscribe("off");
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
  if(!client.loop()) {
    client.connect("LedStrip");
  }

  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 
  
  if (theme == "horror") {  
    // do some periodic updates
    EVERY_N_MILLISECONDS( 20 ) { gHue++; }
  
    uint8_t BeatsPerMinute = 255;
    uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
    for( int i = 0; i < NUM_LEDS; i++) { //9948
      leds[i] = ColorFromPalette(currentPalette, gHue+(i*2), beat-gHue+(i*10));
    }
  } else if (theme == "fire") {
    Fire2012();
  } else if (theme == "pink") {
    static uint8_t startIndex = 0;
    startIndex = startIndex + 1; /* motion speed */
      
    FillLEDsFromPaletteColors( startIndex);
    addGlitter(160);
    
  } else if (theme == "off") {
    fill_solid( leds, NUM_LEDS, CRGB::Black);
  }
  
 
}

void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
    uint8_t brightness = 255;
    
    for( int i = 0; i < NUM_LEDS; i++) {
        leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, LINEARBLEND);
        colorIndex += 3;
    }
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

#define COOLING  55
#define SPARKING 120
void Fire2012()
{
  // Step 1.  Cool down every cell a little
    for( int i = 0; i < NUM_LEDS; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= NUM_LEDS - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < NUM_LEDS; j++) {
      CRGB color = HeatColor( heat[j]);
      int pixelnumber;
      if( false ) {
        pixelnumber = (NUM_LEDS-1) - j;
      } else {
        pixelnumber = j;
      }
      leds[pixelnumber] = color;
    }
}
