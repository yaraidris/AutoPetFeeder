//Yara Idris 400393007
//Luc Suzuki 400332170
//Shehab Ahmed 400359237
//Erin Ng 400360728
#include "Lab_Project.h"
bool Connect() {
  // Connect to MQTT Server and subscribe to the topic
  if (client.connect(clientID, mqtt_username, mqtt_password)) {
      client.subscribe(mqtt_topic);
      return true;
    }
    else {
      return false;
  }
}
void setup() {
  Serial.begin(9600); 
  SPI.begin();     
  mfrc522.PCD_Init();   
  myServo.attach(SERVOPIN,500,2400);
  myServo.write(0); //servo start position
  currentServoPos = 0;
  pinMode(TRIGPIN, OUTPUT);
  pinMode(ECHOPIN, INPUT);
  //Start the WiFi driver and tell it to connect to your local network
  //WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  //While it is connecting, print a '.' to the serial monitor every 500 ms
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //Once connected, print the local IP address
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  //Set the MQTT client to connect to the desired broker
  client.setServer(brokerAddress, addressPort);
  client.setCallback(ReceivedMessage);
  if (Connect()) {
    Serial.println("Connected Successfully to MQTT Broker!");  
  }
  else {
    Serial.println("Connection Failed!");
  }
  
}


void reconnect() {
  //While the client remains unconnected from the MQTT broker, attempt to reconnect every 2 seconds
  //Also, print diagnostic information
  while (!client.connected()) {
    Serial.print("\nAttempting MQTT connection...");
  
    if (client.connect("ESP8266Client")) {
      Serial.println("Connected to MQTT server");  
      client.subscribe("testTopic");
    } else {
      Serial.print("\nFailed to connect to MQTT server, rc = ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

long getUSReading(){ // Returns the current proximity read by the ultrasonic sensor
      digitalWrite(TRIGPIN, LOW);
      delayMicroseconds(2);
      digitalWrite(TRIGPIN, HIGH);
      delayMicroseconds(10);
      digitalWrite(TRIGPIN, LOW);
      return pulseIn(ECHOPIN, HIGH); // Returns the distance read
}
// changes direction of servo spin to dispense the food
int ServoPos(int currentServoPos){
  if (currentServoPos == 0){
    myServo.write(180);
    currentServoPos = 180;
  }
  else{
    myServo.write(0);
    currentServoPos = 0;
  }
  return currentServoPos;
}
//calculates the percentage of food present within the feeder
float PercentFull(){
  float PercentFull = ((Level-MinLevel)/(MaxLevel-MinLevel))*100;
  return PercentFull;
}
//reads message that is subscribed to through MQTT 
void ReceivedMessage(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message received on topic: ");
    Serial.println(topic);
    Serial.print("Message:");
    for (unsigned int i = 0; i < length; i++) {
        Serial.print((char) payload[i]);
    }
    Serial.println();
    Serial.println("-----------------------");
    if ((char)payload[0] == 't') {
      if (currentServoPos == 180){
        myServo.write(0);
        currentServoPos = 0;
      }
      else{
        myServo.write(180);
        currentServoPos = 180;
      }
      client.publish("4ID3_Group3/level", String(Level).c_str());
      client.publish("4ID3_Group3/percent", String(Percentage).c_str());
    }
}
    
void loop() {  
  //updates system values   
  Level = (10-(cmRatio*getUSReading()));
  Percentage = PercentFull();
  //reconnects to MQTT if disconnected
  client.loop();
    //If the client disconnects from the MQTT broker, attempt to reconnect
    if (!client.connected()) {
      reconnect();
    }
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  //Read UID Value
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) {
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
  //checks if correct RFID sensor is read and dispenses food if correct 
  if (content.substring(1) == "FF FC 7C C2") {
    Serial.println("Authorized access");
    Serial.println();
    delay(500);
    currentServoPos = ServoPos(currentServoPos);
    delay(500);
    client.loop();
    //If the client disconnects from the MQTT broker, attempt to reconnect
    if (!client.connected()) {
      reconnect();
    }
    if(!client.loop())
    client.connect("ESP8266Client");
    //Publish the data to the associated topics
    client.publish("4ID3_Group3/level", String(Level).c_str());
    client.publish("4ID3_Group3/percent", String(Percentage).c_str());
    Serial.println("Published data.");
  }
 //no food is dispensed if it is incorrect rfid card
 else   {
    Serial.println("Access denied");
  }
} 
