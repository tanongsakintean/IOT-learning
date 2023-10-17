/*************************************************************

  This is a simple demo of sending and receiving some data.
  Be sure to check out other examples!
 *************************************************************/

/* Fill-in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID           "TMPL6-EY52YSl"
#define BLYNK_TEMPLATE_NAME         "Quickstart Template"
#define BLYNK_AUTH_TOKEN            "Cl3MrF2TJrSFeZ6ZMIj6HiMKYPsQCNf2"

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial


#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <TridentTD_LineNotify.h>
#include "ThingSpeak.h"
#define DHTPIN 26
#define DHTTYPE DHT22  //DHT11, DHT22 type of Sensor
// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "SetzeroDev-2.4G";
char pass[] = "51552105315";
unsigned long myChannelNumber = 2309025;          //เลข ID
const char* myWriteAPIKey = "W23CRFUYCF53MPZC";  //API KEY
#define LINE_TOKEN  "ayj6l9uPBOZwRhHQHzfyel9Njkc2JW6SIsPr62aiLGg" //Line Token
//----------------------------------------Host &fhttpsPort
const char* host = "script.google.com";
const int httpsPort = 443;
//----------------------------------------
const int LED1 = 16;
const int LED2 = 17;
WiFiClientSecure client;  //--> Create a WiFiClientSecure object.
WiFiClient client2;
BlynkTimer timer;

String GAS_ID = "AKfycbyTqIMfbZCD5PvFi4S7GBJ9gs689CcsibCRvMfcfRknkVRzLZv7j4DgSQ1i8pyQeXN4PQ";  //--> spreadsheet script ID
DHT dht(DHTPIN, DHTTYPE);
// This function is called every time the Virtual Pin 0 state changes
BLYNK_WRITE(V4)
{
  // Set incoming value from pin V0 to a variable
  int value = param.asInt();

  // Update state
  Blynk.virtualWrite(V2, value);
  digitalWrite(LED1,value);
}

BLYNK_WRITE(V5)
{
  // Set incoming value from pin V0 to a variable
  int value = param.asInt();

  // Update state
  Blynk.virtualWrite(V3, value);
  digitalWrite(LED2,value);
}

// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED()
{
  // Change Web Link Button message to "Congratulations!"
  Blynk.setProperty(V3, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
  Blynk.setProperty(V3, "onImageUrl",  "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
  Blynk.setProperty(V3, "url", "https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstart-device-was-made");
}

// This function sends Arduino's uptime every second to Virtual Pin 2.
void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);
  Serial.print("temp : ");
  Serial.println(t);
  
    if(t > 24){
    // digitalWrite(LED1,1);
    // digitalWrite(LED2,1);
    Blynk.virtualWrite(V3, 1);
    digitalWrite(LED2,1);
    Blynk.virtualWrite(V2, 1);
    digitalWrite(LED1,1);
    LINE.notify("อุณหภูมิขณะนี้ "+String(t)+" องศา"); 
    LINE.notify("ความชื้นขณะนี้ "+String(h)+" %");
  }

  sendData(t, h);

}

void setup()
{
  // Debug console
  pinMode(LED1,OUTPUT);
  pinMode(LED2,OUTPUT);
  Serial.begin(115200);
  ThingSpeak.begin(client2);  // Initialize ThingSpeak
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  // You can also specify server:
  //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, IPAddress(192,168,1,100), 8080);

  // Setup a function to be called every second
  timer.setInterval(1000L, myTimerEvent);
  dht.begin();
  


  Serial.println("");
  Serial.print("Successfully connected to : ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  LINE.setToken(LINE_TOKEN);  // กำหนด Line Token
}

void loop()
{
  Blynk.run();
  timer.run();

}

void sendData(float value, float value2) {
  
  delay(30000);
  Serial.print("field1 == ");
  Serial.println(value);
  Serial.print("field2 == ");
  Serial.println(value2);
  ThingSpeak.setField(1, value);
  ThingSpeak.setField(2, value2);

  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  if (x == 200) {
    Serial.println("Channel update successful.");
  } else {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  client.setInsecure();
  Serial.println("==========");
  Serial.print("connecting to ");
  Serial.println(host);
  //----------------------------------------Connect to Google host
  if (!client.connect(host, httpsPort)) {

    Serial.println("connection failed");
    return;
  }
  //----------------------------------------
  //----------------------------------------Proses dan kirim data
  float string_temp = value;
  float string_humi = value2;
  String url = "/macros/s/" + GAS_ID + "/exec?temp=" + string_temp + "&humi=" + string_humi;
  // 2 variables
  Serial.print("requesting URL: ");
  Serial.println(url);
  client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "User-Agent: BuildFailureDetectorESP32\r\n" + "Connection: close\r\n\r\n");
  Serial.println("request sent");
  //----------------------------------------
  //---------------------------------------
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
    Serial.println("esp32/Arduino CI successfull!");
  } else {
    Serial.println("esp32/Arduino CI has failed");
  }
  Serial.print("reply was : ");
  Serial.println(line);


  

  Serial.println("closing connection");
  Serial.println("==========");
  Serial.println();
  //----------------------------------------
}
