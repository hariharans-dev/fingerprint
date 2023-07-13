#include <SoftwareSerial.h>
#include <Adafruit_Fingerprint.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <UrlEncode.h>

#define RX_PIN 4
#define TX_PIN 5

const char* ssid = "hariharans";
const char* password = "12345678";
String serverAddress = "http://192.168.123.96:8000/req"; 

SoftwareSerial mySerial(RX_PIN, TX_PIN);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

uint8_t id;

uint8_t getFingerprintEnroll()
{
  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #");
  Serial.println(id);
  while (p != FINGERPRINT_OK)
  {
    p = finger.getImage();
    switch (p)
    {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        sendMessagetext("Image taken");
        delay(1000);
        break;
      case FINGERPRINT_NOFINGER:
        Serial.println(".");
        sendMessagetext("Place finger");
        delay(1000);
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        sendMessagetext("Communication error");
        delay(1000);
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        sendMessagetext("Imaging error");
        delay(1000);
        break;
      default:
        Serial.println("Unknown error");
        sendMessagetext("Unknown error");
        delay(1000);
        break;
    }
  }

  p = finger.image2Tz(1);
  switch (p)
  {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      sendMessagetext("Image converted");
      delay(1000);
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      sendMessagetext("Image too messy");
      delay(1000);
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      sendMessagetext("Communication error");
      delay(1000);
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      sendMessagetext("Could not find fingerprint features");
      delay(1000);
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      sendMessagetext("Could not find fingerprint features");
      delay(1000);
      return p;
    default:
      Serial.println("Unknown error");
      sendMessagetext("Unknown error");
      delay(1000);
      return p;
  }

  Serial.println("Remove finger");
  sendMessagetext("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER)
  {
    p = finger.getImage();
  }
  Serial.print("ID ");
  Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  sendMessagetext("Place same finger again");
  delay(1000);
  while (p != FINGERPRINT_OK)
  {
    p = finger.getImage();
    switch (p)
    {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        sendMessagetext("Image taken");
        delay(1000);
        break;
      case FINGERPRINT_NOFINGER:
        Serial.print(".");
        sendMessagetext("Place your finger");
        delay(1000);
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        sendMessagetext("Communication error");
        delay(1000);
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        sendMessagetext("Imaging error");
        delay(1000);
        break;
      default:
        Serial.println("Unknown error");
        sendMessagetext("Unknown error");
        delay(1000);
        break;
    }
  }

  p = finger.image2Tz(2);
  switch (p)
  {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      sendMessagetext("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      sendMessagetext("Image too messy");
      delay(1000);
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      sendMessagetext("Communication error");
      delay(1000);
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      sendMessagetext("Could not find fingerprint features");
      delay(1000);
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      sendMessagetext("Could not find fingerprint features");
      delay(1000);
      return p;
    default:
      Serial.println("Unknown error");
      sendMessagetext("Unknown error");
      delay(1000);
      return p;
  }

  Serial.print("Creating model for #");
  sendMessagetext("Creating model");
  delay(1000);
  Serial.println(id);

  p = finger.createModel();
  if (p == FINGERPRINT_OK)
  {
    Serial.println("Prints matched!");
    sendMessagetext("Prints matched!");
    delay(1000);
  }
  else if (p == FINGERPRINT_PACKETRECIEVEERR)
  {
    Serial.println("Communication error");
    sendMessagetext("Communication error");
    delay(1000);
    return p;
  }
  else if (p == FINGERPRINT_ENROLLMISMATCH)
  {
    Serial.println("Fingerprints did not match");
    sendMessagetext("Fingerprints did not match");
    delay(1000);
    return p;
  }
  else
  {
    Serial.println("Unknown error");
    sendMessagetext("Unknown error");
    delay(1000);
    return p;
  }

  Serial.print("ID ");
  Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK)
  {
    Serial.println("Stored!");
    sendMessagetext("Fingerprint Stored!");
    delay(1000);
  }
  else if (p == FINGERPRINT_PACKETRECIEVEERR)
  {
    Serial.println("Communication error");
    sendMessagetext("Communication error");
    delay(1000);
    return p;
  }
  else if (p == FINGERPRINT_BADLOCATION)
  {
    Serial.println("Could not store in that location");
    sendMessagetext("Could not store in that location");
    delay(1000);
    return p;
  }
  else if (p == FINGERPRINT_FLASHERR)
  {
    Serial.println("Error writing to flash");
    sendMessagetext("Error writing to flash");
    delay(1000);
    return p;
  }
  else
  {
    Serial.println("Unknown error");
    sendMessagetext("Unknown error");
    delay(1000);
    return p;
  }

  return true;
}

String sendMessage(String message){
  String url = serverAddress + "?text=" + urlEncode(message); // Modify the URL to match your server's endpoint
  WiFiClient client;    
  HTTPClient http;
  http.begin(client, url);
  String response;
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpResponseCode = http.GET(); // Send a GET request instead of POST
  if (httpResponseCode == 200){
    response = http.getString(); // Read the response content
    Serial.println("Message sent successfully.");
  }
  else{
    Serial.println("Error sending the message");
    Serial.println("HTTP response code: ");
    Serial.println(httpResponseCode);
  }
  http.end();
  return response;
}

void sendMessagetext(String message){
  String url = "http://192.168.123.96:8000/msg?text=" + urlEncode(message); // Modify the URL to match your server's endpoint
  WiFiClient client;    
  HTTPClient http;
  http.begin(client, url);
  String response;
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpResponseCode = http.GET(); // Send a GET request instead of POST
  if (httpResponseCode == 200){
    Serial.println("text sent successfully.");
  }
  else{
    Serial.println("Error sending the message");
    Serial.println("HTTP response code: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}





void setup()
{
  Serial.begin(9600);
  while (!Serial);  // For boards like NodeMCU, Wemos, etc.
  delay(100);
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.println("\n\nAdafruit Fingerprint sensor enrollment");
  finger.begin(57600);

  if (finger.verifyPassword())
  {
    Serial.println("Found fingerprint sensor!");
  }
  else
  {
    Serial.println("Did not find fingerprint sensor :(");
    while (1)
    {
      delay(1);
    }
  }

  Serial.println(F("Reading sensor parameters"));
  finger.getParameters();
  Serial.print(F("Status: 0x"));
  Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x"));
  Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: "));
  Serial.println(finger.capacity);
  Serial.print(F("Security level: "));
  Serial.println(finger.security_level);
  Serial.print(F("Device address: "));
  Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: "));
  Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: "));
  Serial.println(finger.baud_rate);
}

void loop()
{
  Serial.println("Ready to enroll a fingerprint!");
  Serial.println("Please type in the ID # (from 1 to 127) you want to save this finger as...");
  String temp=sendMessage("id");
  if(temp!="-1" || temp!="0"){
    Serial.println(temp);
  }
  delay(1000);

  if (temp=="-1" || temp=="0")
  {
    return;
  }
  id=temp.toInt();
  if(id!=0){
    Serial.print("Enrolling ID #");
    Serial.println(id);
    while (!getFingerprintEnroll());
  }
}
