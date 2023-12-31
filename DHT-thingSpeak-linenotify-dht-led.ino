
#include <WiFi.h>
#include <TridentTD_LineNotify.h>
#include "ThingSpeak.h"
#include "DHT.h"

#define DHTPIN 26
#define DHTTYPE DHT22  // DHT 22  (AM2302), AM2321

char ssid[] = "SetzeroDev-2.4G";                        //ชื่อไวไฟ
char pass[] = "51552105315";                       //รหัสไวไฟ
unsigned long myChannelNumber = 2307933;          //เลข ID
const char* myWriteAPIKey = "V2IZNL2ZFHG6FR3S";  //API KEY
// #define LINE_TOKEN  "CBNBojPgU9Fbod07npOQ0idoVSKnmmFfCbP63fjnSLY" //Line Token
const int LED = 16;
DHT dht(DHTPIN, DHTTYPE);
WiFiClient client;

void setup() {
  Serial.begin(115200);
  Serial.println(LINE.getVersion());
  WiFi.mode(WIFI_STA);
  // Connect or reconnect to WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect");
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass);
      delay(5000);
    }
    Serial.println("\nConnected.");
  }
  ThingSpeak.begin(client);  // Initialize ThingSpeak

  Serial.println(F("DHTxx test!"));
  dht.begin();
  // LINE.setToken(LINE_TOKEN);  // กำหนด Line Token
  pinMode(LED,OUTPUT);
}

void loop() {
  // Wait a few seconds between measurements.
  delay(30000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // if(t > 28 ){
  //   digitalWrite(LED,1);
  //   LINE.notify("อุณหภูมิขณะนี้ "+String(t)+" องศา"); 
  //   LINE.notify("ความชื้นขณะนี้ "+String(h)+" %");     
  // }else{
  //    digitalWrite(LED,0);
  // }
  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  ThingSpeak.setField(1, t);
  ThingSpeak.setField(2, h);

  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  if (x == 200) {
    Serial.println("Channel update successful.");
  } else {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.println(F("°F"));
}