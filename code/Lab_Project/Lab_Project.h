//Libraries
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

//Macros
#define DELAY_BETWEEN_SAMPLES_MS 5000
#define SS_PIN 15
#define RST_PIN 2

//Global Variables
const char* ssid = "";
const char* pass = "";
const char* brokerAddress = "broker.hivemq.com";
const char* mqtt_topic = "4ID3_Group3/dispense";
uint16_t addressPort = 1883;
const char* clientID = "ESP01";
const char* mqtt_username = "";
const char* mqtt_password = "";

const int TRIGPIN = 4, ECHOPIN = 5, SERVOPIN = 0;
MFRC522 mfrc522(SS_PIN, RST_PIN); 
Servo myServo; 
const float cmRatio = 0.01723; // Conversion ratio to cm
const int MinLevel = 0, MaxLevel = 10;
int currentServoPos;
float Percentage;
float Level;
int foodDispensed;

//Instantiate MQTT Client
WiFiClient espClient;
PubSubClient client(espClient);