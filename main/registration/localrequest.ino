#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <UrlEncode.h>

const char* ssid = "hariharans";
const char* password = "12345678";
String serverAddress = "http://192.168.123.96:8000/req"; // Replace with your server's IP address or hostname

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

void setup() {
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

void loop() {
  String id=sendMessage("id");
  if(id!="-1"){
    Serial.println(id);
  }
  delay(3000);
}
