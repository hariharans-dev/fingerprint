#include <SoftwareSerial.h>
#include <Adafruit_Fingerprint.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <UrlEncode.h>
#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
#define RELAY_PIN       D5
#define ACCESS_DELAY    1000
#define RX_PIN 4
#define TX_PIN 5
#else
#define mySerial Serial1
#endif

uint8_t id;
int count=0;
String decide="-1";
int set_reg=0;
int set_rec=0;
int exec=0;

const char* ssid = "hariharans";
const char* password = "12345678";
String serverAddress = "http://192.168.123.96:8000/req"; 
String phoneNumber = "+919360745166";
String apiKey = "1717993";

SoftwareSerial mySerial(RX_PIN, TX_PIN);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

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
}

void loop()
{
  String temp=sendMessagedecide("decide");
  if(temp!="-1"){
    Serial.println(temp);
    decide=temp;
  }
  delay(1000);
  if (temp=="-1" && exec==0)
  {
    return;
  }
  exec++;
  if(decide=="0"){
    if(set_reg==0){
      registration_setup();
      Serial.println("registration setup");
      set_reg++;
    }
    registration_loop();
    Serial.println("registration loop");
  }
  else{
    if(set_rec==0){
      recognition_setup();
      Serial.println("recognition setup");
      set_rec++;
    }
    recognition_loop();
    Serial.println("recognition loop");
  }
}

void registration_setup()
{
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

void registration_loop()
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

void recognition_setup()
{
  while (!Serial); 
  delay(100);
  Serial.println("\n\nAdafruit finger detect test");
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  Serial.println(F("Reading sensor parameters"));
  finger.getParameters();
  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
  Serial.print(F("Security level: ")); Serial.println(finger.security_level);
  Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);

  finger.getTemplateCount();

  if (finger.templateCount == 0) {
    Serial.print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
    sendMessagerectext("Sensor doesnt contain any fingerprint data");
  }
  else {
    Serial.println("Waiting for valid finger...");
    sendMessagerectext("Waiting for valid finger....");
    Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  }
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);
}

void recognition_loop()
{
  getFingerprintID();
  delay(50); 
}

void sendwhatsapp(String message){
  String url = "http://api.callmebot.com/whatsapp.php?phone=" + phoneNumber + "&apikey=" + apiKey + "&text=" + urlEncode(message);
  WiFiClient client;    
  HTTPClient http;
  http.begin(client, url);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpResponseCode = http.POST(url);
  Serial.println(httpResponseCode);
  if (httpResponseCode == 200){
    Serial.print("Message sent successfully");
  }
  else{
    Serial.println("Error sending the message");
    Serial.print("HTTP response code: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      sendMessagerectext("Imgae taken");
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      sendMessagerectext("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      sendMessagerectext("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      sendMessagerectext("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      sendMessagerectext("Unknown error");
      return p;
  }
  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      sendMessagerectext("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      sendMessagerectext("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      sendMessagerectext("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      sendMessagerectext("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      sendMessagerectext("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      sendMessagerectext("Unknown error");
      return p;
  }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
    sendMessagerectext("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    sendMessagerectext("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    sendMessagerectext("Did no find a match");
    if(count==3){
      sendwhatsapp("Multiple wrong fingerprint found!");
      sendMessagerectext("Multiple wrong recognition found!!");
      count=0;
    }
    count=count+1;
    return p;
  } else {
    Serial.println("Unknown error");
    sendMessagerectext("Unknown error");
    return p;
  }

  // found a match!
  int idata=finger.fingerID;
  Serial.print("Found ID # "); Serial.print(idata);
  sendMessagerectext("id "+String(idata));
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  digitalWrite(RELAY_PIN, LOW);
  delay(ACCESS_DELAY);
  digitalWrite(RELAY_PIN, HIGH);

  return finger.fingerID;
}

int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
  int idata=finger.fingerID;
  Serial.print("Found ID #"); Serial.print(idata);
  sendMessagerectext("id "+String(idata));
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID;
}

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

String sendMessagedecide(String message){
  String url = "http://192.168.123.96:8000/decidedata?text=" + urlEncode(message); // Modify the URL to match your server's endpoint
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

void sendMessagerectext(String message){
  String url = "http://192.168.123.96:8000/rec?text=" + urlEncode(message); // Modify the URL to match your server's endpoint
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